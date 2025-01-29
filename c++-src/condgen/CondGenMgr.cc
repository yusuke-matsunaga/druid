
/// @file CondGenMgr.cc
/// @brief CondGenMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CondGenMgr.h"
#include "CondGen.h"
#include "CnfGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FFRFaultList.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "OpBase.h"
#include "ym/Timer.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CondGenMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障検出条件を求める．
vector<DetCond>
CondGenMgr::root_cond(
  const TpgNetwork& network,
  SizeType limit,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);
  vector<DetCond> cond_list;
  cond_list.reserve(network.ffr_num());
  for ( auto ffr: network.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " / "
	      << network.ffr_num() << endl;
    }

    CondGen gen{network, ffr, option};
    auto cond = gen.root_cond(limit);
    cond_list.push_back(cond);
  }
  return cond_list;
}

// @brief FFRの故障伝搬条件を表すCNFのサイズを求める．
CnfSize
CondGenMgr::calc_root_cond_size(
  const TpgNetwork& network,
  SizeType limit,
  const JsonValue& option,
  const JsonValue& option2
)
{
  auto total_size = CnfSize::zero();
  for ( auto ffr: network.ffr_list() ) {
    CondGen gen{network, ffr, option};
    auto cond = gen.root_cond(limit);
    if ( cond.cube_list().size() == limit ) {
      // オーバーフローした場合は本当の式を作る．
      auto root = ffr->root();
      StructEngine engine0{network};
      engine0.make_cnf({root}, {root});
      auto size0 = engine0.solver().cnf_size();
      StructEngine engine1{network};
      auto bd_enc = new BoolDiffEnc{engine1, root};
      engine1.make_cnf({}, {root});
      auto size1 = engine1.solver().cnf_size();
      auto size = size1 - size0;
      total_size += size;
    }
    else {
      auto size1 = CnfGen::calc_cnf_size(cond, option2);
      auto size2 = CnfGen::calc_cnf_size(cond);
      if ( size1.literal_num < size2.literal_num ) {
	total_size += size1;
      }
      else {
	total_size += size2;
      }
    }
  }
  return total_size;
}

// @brief 故障検出条件を求める．
vector<DetCond>
CondGenMgr::fault_cond(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  SizeType limit,
  const JsonValue& option
)
{
  int debug = OpBase::get_debug(option);
  vector<DetCond> cond_array(network.max_fault_id());
  FFRFaultList ffr_fault_list{network, fault_list};
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    if ( debug > 1 ) {
      DBG_OUT << "FFR#" << ffr->id()
	      << " [" << ffr_fault_list.fault_list(ffr).size() << "]"
	      << " / "
	      << ffr_fault_list.ffr_list().size() << endl;
    }

    CondGen gen{network, ffr, option};
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto cond = gen.fault_cond(fault, limit);
      cond_array[fault->id()] = cond;
    }
  }
  return cond_array;
}

END_NAMESPACE_DRUID
