
/// @file FaultGroup.cc
/// @brief FaultGroup の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroup.h"
#include "types/TpgNetwork.h"
#include "fsim/Fsim.h"
#include "dtpg/DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultGroup
//////////////////////////////////////////////////////////////////////

// @brief 故障リストとテストベクタを指定したコンストラクタ
FaultGroup
FaultGroup::make(
  const TpgFaultList& fault_list,
  const TestVector& tv,
  const ConfigParam& option
)
{
  auto network = fault_list.network();
  auto tv_assign_list = AssignList(network, tv);
  auto dtpg_option = option.get_param("dtpg");
  AssignList gtc;
  for ( auto fault: fault_list ) {
    /*DD*/std::cout << fault.str() << std::endl;
    auto ffr = network.ffr(fault);
    DtpgEngine engine(ffr, dtpg_option);
    auto lits1 = engine.conv_to_literal_list(tv_assign_list);
    auto lits2 = engine.make_detect_condition(fault);
    auto lits = lits1;
    lits.insert(lits.end(), lits2.begin(), lits2.end());
    auto res = engine.solver().solve(lits);
    if ( res != SatBool3::True ) {
      throw std::logic_error{"something wrong"};
    }
    auto model = engine.solver().model();
    auto cond = engine.extract_sufficient_condition(fault, model);
    auto cond1 = cond.main_cond() + cond.aux_cond();
    { /*DD*/
      std::cout << "GTC:     " << gtc << std::endl
		<< "cond1:   " << cond1 << std::endl;
    }
    gtc.merge(cond1);
    { /*DD*/
      std::cout << "new GTC: " << gtc << std::endl;
    }
  }
  return FaultGroup(fault_list, gtc);
}

END_NAMESPACE_DRUID
