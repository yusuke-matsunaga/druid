
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMgr.h"
#include "FFREngine.h"
#include "MFFCEngine.h"
#include "TpgFFR.h"
#include "TpgMFFC.h"
#include "TpgFaultStatusMgr.h"
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

// 一つの故障に対するテスト生成を行う．
void
gen_pattern(
  DtpgEngine& engine,
  const TpgFault* fault,
  TpgFaultStatusMgr& status_mgr,
  DtpgStats& stats,
  FaultTvCallback det_func,
  FaultCallback untest_func,
  FaultCallback abort_func
)
{
  Timer timer;
  timer.start();
  auto ans = engine.solve(fault);
  timer.stop();
  auto sat_time = timer.get_time();

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    timer.reset();
    timer.start();
    auto testvect = engine.gen_pattern(fault);
    timer.stop();
    auto backtrace_time = timer.get_time();

    det_func(fault, testvect);
    status_mgr.set_status(fault, FaultStatus::Detected);
    stats.update_det(sat_time, backtrace_time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    untest_func(fault);
    status_mgr.set_status(fault, FaultStatus::Untestable);
    stats.update_untest(sat_time);
  }
  else { // SatBool3::X
    // つまりアボート
    abort_func(fault);
    stats.update_abort(sat_time);
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
DtpgStats
DtpgMgr::run(
  const TpgNetwork& network,
  TpgFaultStatusMgr& status_mgr,
  FaultTvCallback det_func,
  FaultCallback untest_func,
  FaultCallback abort_func,
  const JsonValue& option
)
{
  string dtpg_type = "ffr";
  bool multi = false;
  bool dchain = true;
  string ex_mode = "simple";
  string just_mode = "just2";
  SatInitParam init_param;
  // option の解析
  if ( !option.is_object() ) {
    // エラー
    throw std::invalid_argument{"option should be a JsonObject"};
  }
  else {
    get_string(option, "dtpg_type", dtpg_type);
    get_bool(option, "multi_thread", multi);
    get_bool(option, "dchain", dchain);
    get_string(option, "extract_mode", ex_mode);
    get_string(option, "justify_mode", just_mode);
    if ( option.has_key("sat_param") ) {
      init_param = SatInitParam{option.at("sat_param")};
    }
  }

  // ノード番号をキーにして関係する故障のリストを格納する配列
  vector<vector<const TpgFault*>> node_fault_list_array(network.node_num());
  for ( auto fault: status_mgr.fault_list() ) {
    auto node = fault->origin_node();
    node_fault_list_array[node->id()].push_back(fault);
  }

  DtpgStats stats;
  if ( dtpg_type == "ffr" ) {
    // FFR 単位で処理を行う．
    for ( auto ffr: network.ffr_list() ) {
      // ffr に関係する故障を集める．
      vector<const TpgFault*> fault_list;
      for ( auto node: ffr->node_list() ) {
	for ( auto fault: node_fault_list_array[node->id()] ) {
	  if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	    fault_list.push_back(fault);
	  }
	}
      }
      if ( fault_list.empty() ) {
	continue;
      }
      FFREngine engine{network, ffr, dchain, ex_mode, just_mode, init_param};
      engine.make_cnf();
      for ( auto fault: fault_list ) {
	// 途中で status が変化している場合があるので再度チェック
	if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	  gen_pattern(engine, fault, status_mgr, stats,
		      det_func, untest_func, abort_func);
	}
      }
    }
  }
  else if ( dtpg_type == "mffc" ) {
    // MFFC 単位で処理を行う．
    const TpgFFR* ffr1 = nullptr;
    bool ffr_mode = true;
    for ( auto mffc: network.mffc_list() ) {
      vector<const TpgFault*> fault_list;
      for ( auto ffr: mffc->ffr_list() ) {
	SizeType nf = 0;
	for ( auto node: ffr->node_list() ) {
	  for ( auto fault: node_fault_list_array[node->id()] ) {
	    if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	      fault_list.push_back(fault);
	      ++ nf;
	    }
	  }
	}
	if ( nf > 0 ) {
	  if ( ffr1 == nullptr ) {
	    ffr1 = ffr;
	  }
	  else {
	    ffr_mode = false;
	  }
	}
      }
      if ( fault_list.empty() ) {
	continue;
      }
      if ( ffr_mode ) {
	FFREngine engine{network, ffr1, dchain, ex_mode, just_mode, init_param};
	engine.make_cnf();
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合があるので再度チェックする．
	  if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	    gen_pattern(engine, fault, status_mgr, stats,
			det_func, untest_func, abort_func);
	  }
	}
      }
      else {
	MFFCEngine engine{network, mffc, dchain, ex_mode, just_mode, init_param};
	engine.make_cnf();
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合があるので再度チェックする．
	  if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	    gen_pattern(engine, fault, status_mgr, stats,
			det_func, untest_func, abort_func);
	  }
	}
      }
    }
  }
  else {
    ostringstream buf;
    buf << dtpg_type << ": unknown value for 'dtpg_type'";
    throw std::invalid_argument{buf.str()};
  }

  return stats;
}

END_NAMESPACE_DRUID
