
/// @file MpComp_Expand.cc
/// @brief MpComp_Expand の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Expand.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"
#include "dtpg/DtpgEngine.h"
#include "MpAnalyze.h"
#include "FaultGroup.h"
#include "ym/MinCov.h"
#include "ym/UdGraph.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
static int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Expand
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_Expand::MpComp_Expand()
{
}

// @brief デストラクタ
MpComp_Expand::~MpComp_Expand()
{
}

BEGIN_NONAMESPACE

TestVector
expand(
  const TestVector& tv,
  const TpgFaultList& fault_list
)
{
  auto network = fault_list.network();
  auto tv_assign_list = network.assign_list(tv);
  if ( debug ) {
    std::cout << "==============================" << std::endl
	      << "expand" << std::endl
	      << "TV:    " << tv_assign_list << std::endl
	      << "Faults:" << std::endl;
    for ( auto fault: fault_list ) {
      std::cout << "   " << fault.str() << std::endl;
    }
    std::cout << "==============================" << std::endl;
  }
  AssignList new_assign_list;
  bool error = false; /*DD*/
  for ( auto fault: fault_list ) {
    auto root = fault.ffr_root();
    auto ffr = network.ffr(root);
    DtpgEngine engine(ffr);
    auto lits = engine.make_detect_condition(fault);
    auto tv_lits = engine.conv_to_literal_list(tv_assign_list);
    lits.insert(lits.end(), tv_lits.begin(), tv_lits.end());
    auto res = engine.solver().solve(lits);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"something wrong"};
    }
    auto& model = engine.solver().model();
    auto cond = engine.extract_sufficient_condition(fault, model, tv_assign_list);
    auto pi_assign = engine.justify(cond, model, tv_assign_list);
    if ( debug ) {
      auto tmp = pi_assign - tv_assign_list;
      if ( tmp.size() > 0 ) {
	std::cout << "--------------------------------" << std::endl;
	std::cout << "Fault:        " << fault.str() << std::endl
		  << "pi_assign:    " << pi_assign << std::endl
		  << "tv_assign:    " << tv_assign_list << std::endl
		  << "extra assign: " << tmp << std::endl;
	std::cout << "--------------------------------" << std::endl;
	error = true;
      }
    }
    new_assign_list.merge(pi_assign);
  }
  if ( error ) {
    network.print(std::cout);
    abort();
  }
  auto new_tv = TestVector(new_assign_list);
  return new_tv;
}

END_NONAMESPACE

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Expand::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  MpAnalyze analyze(tv_list, fault_list, option);

  auto pos_list = analyze.ex_pos_list();

  // この順番に故障検出の担当するベクタを決める．
  // -> 担当でない故障を削除する．
  // -> 結果，テストベクタを更新する．
  auto network = fault_list.network();
  std::vector<bool> mark(network.max_fault_id(), false);
  std::vector<TestVector> new_tv_list;
  new_tv_list.reserve(tv_list.size());
  for ( auto pos: pos_list ) {
    TpgFaultList new_fault_list;
    for ( auto fid: analyze.det_list(pos) ) {
      if ( !mark[fid] ) {
	mark[fid] = true;
	auto fault = network.fault(fid);
	new_fault_list.push_back(fault);
      }
    }
    if ( !new_fault_list.empty() ) {
      auto new_tv = expand(tv_list[pos], new_fault_list);
      new_tv_list.push_back(new_tv);
    }
  }
  return new_tv_list;
}

END_NAMESPACE_DRUID
