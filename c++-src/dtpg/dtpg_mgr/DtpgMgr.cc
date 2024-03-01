
/// @file DtpgMgr.cc
/// @brief DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMgr.h"
#include "DtpgDriver.h"
#include "DtpgResult.h"
#include "DetectOp.h"
#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgMgr::DtpgMgr(
  const TpgNetwork& network,
  TpgFaultMgr& fault_mgr,
  const JsonValue& option,
  bool multi
) : mNetwork{network},
    mFaultMgr{fault_mgr},
    mDriver{DtpgDriver::new_driver(*this, network,
				   mFaultMgr.fault_type() == FaultType::TransitionDelay,
				   option)}
{
  mFsim.initialize(network, mFaultMgr.fault_type(), true, multi);
  mFsim.set_fault_list(mFaultMgr.rep_fault_list());
  if ( option.is_object() ) {
    if ( option.has_key("dop") ) {
      auto dop_obj = option.at("dop");
      if ( dop_obj.is_array() ) {
	SizeType n = dop_obj.size();
	for ( SizeType i = 0; i < n; ++ i ) {
	  auto obj = dop_obj.at(i);
	  add_dop(obj);
	}
      }
      else {
	add_dop(dop_obj);
      }
    }
    if ( option.has_key("uop") ) {
      auto uop_obj = option.at("uop");
      if ( uop_obj.is_array() ) {
	SizeType n = uop_obj.size();
	for ( SizeType i = 0; i < n; ++ i ) {
	  auto obj = uop_obj.at(i);
	  add_uop(obj);
	}
      }
      else {
	add_uop(uop_obj);
      }
    }
  }
}

// @brief デストラクタ
DtpgMgr::~DtpgMgr()
{
  delete mDriver;
}

// @brief テスト生成を行う．
void
DtpgMgr::run()
{
  mStats.clear();

  mDriver->run();
}

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
  if ( type_name == "base" ) {
    op = new_DopBase(fault_mgr());
  }
  else if ( type_name == "drop" ) {
    op = new_DopDrop(fault_mgr(), fsim());
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
  if ( type_name == "base" ) {
    op = new_UopBase(fault_mgr());
  }
  else if ( type_name == "skip" ) {
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
  const TpgFault& fault,
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
  const TpgFault& fault,
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
  const TpgFault& fault,
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

END_NAMESPACE_DRUID
