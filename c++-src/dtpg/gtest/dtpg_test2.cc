
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "dtpg/DtpgEngine.h"
//#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/AssignList.h"
#include "types/FaultType.h"
//#include "types/TestVector.h"
//#include "fsim/Fsim.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

TEST(DtpgTest, test1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s298.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#91:I1:SA0" ) {
      AssignList assign_list;
      assign_list.add(network.node(4), 1, true);
      assign_list.add(network.node(5), 1, false);
      assign_list.add(network.node(6), 1, true);
      assign_list.add(network.node(7), 1, false);
      assign_list.add(network.node(8), 1, false);
      assign_list.add(network.node(10), 1, true);

      auto ffr = network.ffr(fault);
      DtpgEngine engine(ffr);
      auto lits = engine.make_detect_condition(fault);
      auto tv_lits = engine.conv_to_literal_list(assign_list);
      lits.insert(lits.end(), tv_lits.begin(), tv_lits.end());
      auto res = engine.solver().solve(lits);
      if ( res != SatBool3::True ) {
	throw std::logic_error{"something wrong"};
      }
      auto& model = engine.solver().model();
      auto cond = engine.extract_sufficient_condition(fault, model, assign_list);
      auto pi_assign = engine.justify(cond, model, assign_list);
      {
	auto tmp = pi_assign - assign_list;
	if ( tmp.size() > 0 ) {
	  std::cout << "--------------------------------" << std::endl;
	  std::cout << "Fault:        " << fault.str() << std::endl
		    << "pi_assign:    " << pi_assign << std::endl
		    << "tv_assign:    " << assign_list << std::endl
		    << "extra assign: " << tmp << std::endl;
	  std::cout << "--------------------------------" << std::endl;
	  network.print(std::cout);
	}
      }
    }
  }
}

END_NAMESPACE_DRUID
