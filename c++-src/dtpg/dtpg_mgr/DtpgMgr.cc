
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgMgr.h"
#include "dtpg/DtpgResult.h"
#include "DtpgDriver.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/TpgFFR.h"
#include "types/TpgMFFC.h"
#include "types/TpgNode.h"
#include "types/OpBase.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// FFR に関係する故障を求める．
bool
get_faults(
  const DtpgMgr& mgr,
  const TpgFFR& ffr,
  const std::vector<TpgFaultList>& node_fault_list_array,
  TpgFaultList& fault_list
)
{
  bool has_faults = false;
  for ( auto node: ffr.node_list() ) {
    for ( auto fault: node_fault_list_array[node.id()] ) {
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
TpgFFR
get_faults(
  const DtpgMgr& mgr,
  const TpgMFFC& mffc,
  const std::vector<TpgFaultList>& node_fault_list_array,
  TpgFaultList& fault_list
)
{
  bool ffr_mode = true;
  TpgFFR ffr1;
  for ( auto ffr: mffc.ffr_list() ) {
    if ( get_faults(mgr, ffr, node_fault_list_array, fault_list) ) {
      if ( !ffr1.is_valid() ) {
	ffr1 = ffr;
      }
      else {
	ffr_mode = false;
      }
    }
  }
  if ( !ffr_mode ) {
    ffr1 = TpgFFR();
  }
  return ffr1;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgMgr::DtpgMgr(
  const TpgNetwork& network,
  const TpgFaultList& fault_list
) : mNetwork(network),
    mFaultList{fault_list},
    mDtpgResult(fault_list.max_fid() + 1, DtpgResult::undetected())
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
  std::string group_mode = "ffr";
  bool multi = false;
  // option の解析
  OpBase::get_string(option, "group_mode", group_mode);
  OpBase::get_bool(option, "multi_thread", multi);

  // ノード番号をキーにして関係する故障番号のリストを格納する配列
  std::vector<TpgFaultList> node_fault_list_array(mNetwork.node_num());
  for ( auto fault: mFaultList ) {
    auto node = fault.origin_node();
    node_fault_list_array[node.id()].push_back(fault);
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
      auto driver = DtpgDriver::node_driver(*this, node, option);
      for ( auto fault: node_fault_list_array[node.id()] ) {
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
      TpgFaultList fault_list;
      if ( !get_faults(*this, ffr, node_fault_list_array, fault_list) ) {
	continue;
      }

      auto driver = DtpgDriver::ffr_driver(*this, ffr, option);
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
      TpgFaultList fault_list;
      auto ffr = get_faults(*this, mffc, node_fault_list_array, fault_list);
      if ( fault_list.empty() ) {
	continue;
      }
      if ( ffr.is_valid() ) {
	auto driver = DtpgDriver::ffr_driver(*this, ffr, option);
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
	auto driver = DtpgDriver::mffc_driver(*this, mffc, option);
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
    std::ostringstream buf;
    buf << group_mode << ": unknown value for 'group_mode'";
    throw std::invalid_argument{buf.str()};
  }

  return stats;
}

// @brief 故障に対するテスト生成の結果を返す．
DtpgResult
DtpgMgr::dtpg_result(
  const TpgFault& fault
) const
{
  auto fid = fault.id();
  if ( fid >= mDtpgResult.size() ) {
    throw std::out_of_range{"fid is out of range"};
  }
  return mDtpgResult[fid];
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
  const TpgFault& fault,
  DtpgResult result
)
{
  auto fid = fault.id();
  if ( fid >= mDtpgResult.size() ) {
    throw std::out_of_range{"fid is out of range"};
  }
  mDtpgResult[fid] = result;
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
  for ( auto fault: mFaultList ) {
    if ( dtpg_result(fault).status() == FaultStatus::Detected ) {
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
  for ( auto fault: mFaultList ) {
    if ( dtpg_result(fault).status() == FaultStatus::Untestable ) {
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
  for ( auto fault: mFaultList ) {
    if ( dtpg_result(fault).status() == FaultStatus::Undetected ) {
      ++ count;
    }
  }
  return count;
}

END_NAMESPACE_DRUID
