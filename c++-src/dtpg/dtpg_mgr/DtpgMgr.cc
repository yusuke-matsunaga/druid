
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMgr.h"
#include "DtpgDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief テスト生成を行う．
DtpgStats
DtpgMgr::run(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  TpgFaultStatusMgr& status_mgr,
  const JsonValue& option,
  FaultTvCallback det_func,
  FaultCallback untest_func,
  FaultCallback abort_func
)
{
  string dtpg_type;
  bool multi = false;
  { // option の解析
    if ( !option.is_object() ) {
      // エラー
    }
    else {
      if ( option.has_key("dtpg_type") ) {
	auto dtpg_type_obj = option.at("dtpg_type");
	if ( dtpg_type_obj.is_string() ) {
	  dtpg_type = dtpg_type_obj.get_string();
	}
	else {
	  throw std::invalid_argument{"'dtpg_type' should be a string"};
	}
      }
      if ( option.has_key("multi_thread") ) {
	auto multi_obj = option.at("multi_thread");
	if ( multi_obj.is_bool() ) {
	  multi = multi_obj.get_bool();
	}
      }
    }
  }

  // ノード番号をキーにして関係する故障のリストを格納する配列
  vector<vector<const TpgFault*>> node_fault_list_array(network.node_num());
  for ( auto fault: fault_list ) {
    auto node = fault->origin_node();
    node_fault_list_array[node->id()].push_back(fault);
  }

  if ( dtpg_type == "ffr" ) {
    // FFR 単位で処理を行う．
    for ( auto ffr: network.ffr_list() ) {
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
      auto driver = DtpgDriver::new_driver(network, ffr, option);
      for ( auto fault: fault_list ) {
	// 途中で status が変化している場合がある．
	if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	  driver->gen_pattern(fault, det_func, untest_func, abort_func);
	}
      }
      delete driver;
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
	auto driver = DtpgDriver::new_driver(network, ffr1, option);
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合がある．
	  if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	    driver->gen_pattern(fault, det_func, untest_func, abort_func);
	  }
	}
	delete driver;
      }
      else {
	auto drier = DtpgDriver::new_driver(network, mffc, option);
	for ( auto fault: fault_list ) {
	  // 途中で status が変化している場合がある．
	  if ( status_mgr.get_status(fault) == FaultStatus::Undetected ) {
	    driver->gen_pattern(fault, det_func, untest_func, abort_func);
	  }
	}
	delete driver;
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

#if 0
/// @brief 組み込み型の DetectOp を登録する．
void
DtpgMgr::add_dop(
  const JsonValue& js_obj
)
{
  string type_name;
  if ( js_obj.is_string() ) {
    type_name = js_obj.get_string();
  }
  else if ( js_obj.is_object() ) {
    if ( js_obj.has_key("type") ) {
      auto type_obj = js_obj.at("type");
      if ( type_obj.is_string() ) {
	type_name = type_obj.get_string();
      }
    }
  }
  if ( type_name == string{} ) {
    throw std::invalid_argument{"invalid JSON object for operator specification"};
  }
  DetectOp* op = nullptr;
#if 0
  if ( type_name == "base" ) {
    op = new_DopBase(*this);
  }
  else
#endif
  if ( type_name == "drop" ) {
    op = new_DopDrop(*this, fsim());
  }
  else if ( type_name == "tvlist" ) {
    op = new_DopTvList(mTVList);
  }
  else if ( type_name == "verify" ) {
    op = new_DopVerify(fsim(), mVerifyResult);
  }
  else {
    ostringstream buf;
    buf << type_name << ": unknown DetectOp name";
    throw std::invalid_argument{buf.str()};
  }
  add_dop(op);
}

// @brief 組み込み型の UntestOp を登録する．
void
DtpgMgr::add_uop(
  const JsonValue& js_obj
)
{
  string type_name;
  if ( js_obj.is_string() ) {
    type_name = js_obj.get_string();
  }
  else if ( js_obj.is_object() ) {
    if ( js_obj.has_key("type") ) {
      auto type_obj = js_obj.at("type");
      if ( type_obj.is_string() ) {
	type_name = type_obj.get_string();
      }
    }
  }
  if ( type_name == string{} ) {
    throw std::invalid_argument{"invalid JSON object for operator specification"};
  }
  UntestOp* op = nullptr;
#if 0
  if ( type_name == "base" ) {
    op = new_UopBase(fault_mgr());
  }
  else
#endif
  if ( type_name == "skip" ) {
    if ( js_obj.is_object() ) {
      if ( js_obj.has_key("threshold") ) {
	auto thr_obj = js_obj.at("threadhold");
	if ( thr_obj.is_int() ) {
	  SizeType thr = thr_obj.get_int();
	  op = new_UopSkip(thr);
	}
      }
    }
    if ( op == nullptr ) {
      throw std::invalid_argument{"'skip' type requires 'threthold' parameter"};
    }
  }
  else {
    ostringstream buf;
    buf << type_name << ": unknown UntestOp name";
    throw std::invalid_argument{buf.str()};
  }
  add_uop(op);
}

// @brief テストパタン生成が成功した時の結果を更新する．
void
DtpgMgr::update_det(
  const TpgFault* fault,
  const TestVector& tv,
  double sat_time,
  double backtrace_time
)
{
  for ( auto dop: mDopList ) {
    (*dop)(fault, tv);
  }
  mStats.update_det(sat_time, backtrace_time);
}

// @brief 冗長故障の特定が行えた時の結果を更新する．
void
DtpgMgr::update_untest(
  const TpgFault* fault,
  double sat_time
)
{
  for ( auto uop: mUopList ) {
    (*uop)(fault);
  }
  mStats.update_untest(sat_time);
}

// @brief アボートした時の結果を更新する．
void
DtpgMgr::update_abort(
  const TpgFault* fault,
  double sat_time
)
{
  mStats.update_abort(sat_time);
}

// @brief CNF 生成に関する情報を更新する．
void
DtpgMgr::update_cnf(
  double time
)
{
  mStats.update_cnf(time);
}

// @brief SATの統計情報を更新する．
void
DtpgMgr::update_sat_stats(
  const SatStats& sat_stats
)
{
  mStats.update_sat_stats(sat_stats);
}
#endif

END_NAMESPACE_DRUID
