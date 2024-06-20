
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMgr.h"
#include "DtpgResult.h"
#include "NodeDriver.h"
#include "FFRDriver.h"
#include "MFFCDriver.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgMFFC.h"
#include "TpgNode.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// bool型のオプションを取り出す．
//
// 結果は value に上書きされる．
// エラーが起こったら std::invalid_argument 例外を送出する．
void
get_bool(
  const JsonValue& option,
  const string& keyword,
  bool& value
)
{
  if ( option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_bool() ) {
      value = value_obj.get_bool();
    }
    else {
      ostringstream buf;
      buf << "'" << keyword << "' should be a bool";
      throw std::invalid_argument{buf.str()};
    }
  }
}

// 文字列型のオプションを取り出す．
//
// 結果は value に上書きされる．
// エラーが起こったら std::invalid_argument 例外を送出する．
void
get_string(
  const JsonValue& option,
  const string& keyword,
  string& value
)
{
  if ( option.has_key(keyword) ) {
    auto value_obj = option.at(keyword);
    if ( value_obj.is_string() ) {
      value = value_obj.get_string();
    }
    else {
      ostringstream buf;
      buf << "'" << keyword << "' should be a string";
      throw std::invalid_argument{buf.str()};
    }
  }
}

// FFR に関係する故障を求める．
bool
get_faults(
  const DtpgMgr& mgr,
  const TpgFFR* ffr,
  const vector<vector<const TpgFault*>>& node_fault_list_array,
  vector<const TpgFault*>& fault_list
)
{
  bool has_faults = false;
  for ( auto node: ffr->node_list() ) {
    for ( auto fault: node_fault_list_array[node->id()] ) {
      if ( mgr.dtpg_result(fault).status() == FaultStatus::Undetected ) {
	fault_list.push_back(fault);
	has_faults = true;
      }
    }
  }
  return has_faults;
}

// MFFC に関係する故障を求める．
//
// 故障が唯一のFFR内にのみ存在する場合にはそのFFRを返す．
const TpgFFR*
get_faults(
  const DtpgMgr& mgr,
  const TpgMFFC* mffc,
  const vector<vector<const TpgFault*>>& node_fault_list_array,
  vector<const TpgFault*>& fault_list
)
{
  bool ffr_mode = true;
  const TpgFFR* ffr1 = nullptr;
  for ( auto ffr: mffc->ffr_list() ) {
    if ( get_faults(mgr, ffr, node_fault_list_array, fault_list) ) {
      if ( ffr1 == nullptr ) {
	ffr1 = ffr;
      }
      else {
	ffr_mode = false;
      }
    }
  }
  if ( !ffr_mode ) {
    ffr1 = nullptr;
  }
  return ffr1;
}

// 故障番号の最大値を得る．
SizeType
max_fid(
  const vector<const TpgFault*>& fault_list
)
{
  SizeType max_id = 0;
  for ( auto fault: fault_list ) {
    max_id = std::max(max_id, fault->id());
  }
  return max_id + 1;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgMgr::DtpgMgr(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
) : mNetwork{network},
    mFaultList{fault_list},
    mDtpgResult(max_fid(fault_list), DtpgResult::undetected())
{
}

// @brief テスト生成を行う．
DtpgStats
DtpgMgr::run(
  Callback_Det det_func,
  Callback_Undet untest_func,
  Callback_Undet abort_func,
  const JsonValue& option
)
{
  string group_mode = "ffr";
  bool multi = false;
  // option の解析
  if ( !option.is_object() ) {
    // エラー
    throw std::invalid_argument{"option should be a JsonObject"};
  }
  else {
    get_string(option, "group_mode", group_mode);
    get_bool(option, "multi_thread", multi);
  }

  // ノード番号をキーにして関係する故障のリストを格納する配列
  vector<vector<const TpgFault*>> node_fault_list_array(mNetwork.node_num());
  for ( auto fault: mFaultList ) {
    auto node = fault->origin_node();
    node_fault_list_array[node->id()].push_back(fault);
  }

  // 結果をクリアしておく．
  mTvList.clear();
  for ( auto& r: mDtpgResult ) {
    r = DtpgResult::undetected();
  }

  DtpgStats stats;
  if ( group_mode == "node" ) {
    // ノード単位で処理を行う．
    for ( auto node: mNetwork.node_list() ) {
      NodeDriver driver{*this, node, option};
      for ( auto fault: node_fault_list_array[node->id()] ) {
	// 途中で status が変化している場合があるので再度チェック
	if ( dtpg_result(fault).status() == FaultStatus::Undetected ) {
	  driver.gen_pattern(fault, stats,
			     det_func, untest_func, abort_func);
	}
      }
      auto cnf_time = driver.cnf_time();
      stats.update_cnf(cnf_time);
      auto sat_stats = driver.sat_stats();
      stats.update_sat_stats(sat_stats);
    }
  }
  else if ( group_mode == "ffr" ) {
    // FFR 単位で処理を行う．
    for ( auto ffr: mNetwork.ffr_list() ) {
      // ffr に関係する故障を集める．
      vector<const TpgFault*> fault_list;
      if ( !get_faults(*this, ffr, node_fault_list_array, fault_list) ) {
	continue;
      }
      FFRDriver driver{*this, ffr, option};
      for ( auto fault: fault_list ) {
	// 途中で status が変化している場合があるので再度チェック
	if ( dtpg_result(fault).status() == FaultStatus::Undetected ) {
	  driver.gen_pattern(fault, stats,
			     det_func, untest_func, abort_func);
	}
      }
      auto cnf_time = driver.cnf_time();
      stats.update_cnf(cnf_time);
      auto sat_stats = driver.sat_stats();
      stats.update_sat_stats(sat_stats);
    }
  }
  else if ( group_mode == "mffc" ) {
    // MFFC 単位で処理を行う．
    for ( auto mffc: mNetwork.mffc_list() ) {
      vector<const TpgFault*> fault_list;
      auto ffr = get_faults(*this, mffc, node_fault_list_array, fault_list);
      if ( fault_list.empty() ) {
	continue;
      }
      if ( ffr != nullptr ) {
	FFRDriver driver{*this, ffr, option};
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合があるので再度チェックする．
	  if ( dtpg_result(fault).status() == FaultStatus::Undetected ) {
	    driver.gen_pattern(fault, stats,
			       det_func, untest_func, abort_func);
	  }
	}
	auto cnf_time = driver.cnf_time();
	stats.update_cnf(cnf_time);
	auto sat_stats = driver.sat_stats();
	stats.update_sat_stats(sat_stats);
      }
      else {
	MFFCDriver driver{*this, mffc, option};
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合があるので再度チェックする．
	  if ( dtpg_result(fault).status() == FaultStatus::Undetected ) {
	    driver.gen_pattern(fault, stats,
			       det_func, untest_func, abort_func);
	  }
	}
	auto cnf_time = driver.cnf_time();
	stats.update_cnf(cnf_time);
	auto sat_stats = driver.sat_stats();
	stats.update_sat_stats(sat_stats);
      }
    }
  }
  else {
    ostringstream buf;
    buf << group_mode << ": unknown value for 'group_mode'";
    throw std::invalid_argument{buf.str()};
  }

  return stats;
}

// @brief 故障に対するテスト生成の結果を返す．
DtpgResult
DtpgMgr::dtpg_result(
  const TpgFault* fault
) const
{
  ASSERT_COND( fault->id() < mDtpgResult.size() );
  return mDtpgResult[fault->id()];
}

// @brief テストパタンを追加する．
void
DtpgMgr::add_testvector(
  const TestVector& tv
)
{
  mTvList.push_back(tv);
}

// @brief 故障に対するテスト生成の結果を設定する．
void
DtpgMgr::set_dtpg_result(
  const TpgFault* fault,
  DtpgResult result
)
{
  ASSERT_COND( fault->id() < mDtpgResult.size() );
  mDtpgResult[fault->id()] = result;
}

// @brief 全故障数を返す．
SizeType
DtpgMgr::total_count() const
{
  return mFaultList.size();
}

// @brief 検出済み故障数を返す．
SizeType
DtpgMgr::detected_count() const
{
  SizeType count = 0;
  for ( auto f: mFaultList ) {
    if ( dtpg_result(f).status() == FaultStatus::Detected ) {
      ++ count;
    }
  }
  return count;
}

// @brief 検出不能故障数を返す．
SizeType
DtpgMgr::untestable_count() const
{
  SizeType count = 0;
  for ( auto f: mFaultList ) {
    if ( dtpg_result(f).status() == FaultStatus::Untestable ) {
      ++ count;
    }
  }
  return count;
}

// @brief 未検出故障数を返す．
SizeType
DtpgMgr::undetected_count() const
{
  SizeType count = 0;
  for ( auto f: mFaultList ) {
    if ( dtpg_result(f).status() == FaultStatus::Undetected ) {
      ++ count;
    }
  }
  return count;
}

END_NAMESPACE_DRUID
