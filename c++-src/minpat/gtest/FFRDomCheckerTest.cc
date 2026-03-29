
/// @file FFRDomCheckerTest.cc
/// @brief FFRDomChecker のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "FFRDomChecker.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/FaultType.h"


BEGIN_NAMESPACE_DRUID

TEST(FFRDomCheckerTest, case1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "b15.bench";
  auto network = TpgNetwork::read_iscas89(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();

  TpgFault fault1;
  TpgFault fault2;
  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#3464:I0:SA1" ) {
      fault1 = fault;
    }
    else if ( fault.str() == "G#3462:I0:SA1" ) {
      fault2 = fault;
    }
  }

  auto ffr = network.ffr(fault1);
  TpgFaultList fault_list1;
  fault_list1.push_back(fault1);
  fault_list1.push_back(fault2);
  std::vector<bool> del_mark(network.max_fault_id(), false);

  FFRDomChecker checker(ffr, fault_list1, del_mark);

  checker.run();

  std::cout << fault1.str() << ": " << del_mark[fault1.id()] << std::endl
	    << fault2.str() << ": " << del_mark[fault2.id()] << std::endl;

  auto root = ffr.root();
  BdEngine engine(root);

  auto pvar = engine.prop_var();
  auto cond1 = fault1.ffr_propagate_condition();
  auto cond2 = fault2.ffr_propagate_condition();
  auto lits1 = engine.conv_to_literal_list(cond1);
  auto lits2 = engine.conv_to_literal_list(cond2);
  lits2.push_back(pvar);
  bool ok = true;
  for ( auto lit: lits1 ) {
    auto tmp_lits = lits2;
    tmp_lits.push_back(~lit);
    auto res = engine.solver().solve(tmp_lits);
    if ( res == SatBool3::True ) {
      ok = false;
      break;
    }
  }
  EXPECT_TRUE( ok );

  AssignList assign_list;
  assign_list.add(network.node(106), 1, true);
  assign_list.add(network.node(107), 1, false);
  assign_list.add(network.node(108), 1, true);
  assign_list.add(network.node(218), 1, false);
  assign_list.add(network.node(219), 1, false);
  assign_list.add(network.node(220), 1, true);
  assign_list.add(network.node(221), 1, true);
  assign_list.add(network.node(222), 1, true);
  assign_list.add(network.node(223), 1, true);
  assign_list.add(network.node(224), 1, false);
  assign_list.add(network.node(225), 1, true);
  assign_list.add(network.node(226), 1, false);
  assign_list.add(network.node(227), 1, false);
  assign_list.add(network.node(228), 1, true);
  assign_list.add(network.node(237), 1, false);
  assign_list.add(network.node(238), 1, false);
  assign_list.add(network.node(239), 1, false);
  assign_list.add(network.node(240), 1, false);
  assign_list.add(network.node(241), 1, true);
  assign_list.add(network.node(242), 1, true);
  assign_list.add(network.node(243), 1, true);
  assign_list.add(network.node(247), 1, false);
  assign_list.add(network.node(248), 1, false);
  assign_list.add(network.node(249), 1, false);
  auto assign_lits = engine.conv_to_literal_list(assign_list);

  auto tmp_lits = lits1;
  tmp_lits.insert(tmp_lits.end(), assign_lits.begin(), assign_lits.end());
  tmp_lits.push_back(pvar);
  auto res1 = engine.solver().solve(tmp_lits);
  EXPECT_EQ( res1, SatBool3::True );

  tmp_lits = lits2;
  tmp_lits.insert(tmp_lits.end(), assign_lits.begin(), assign_lits.end());
  auto res2 = engine.solver().solve(tmp_lits);
  EXPECT_EQ( res2, SatBool3::True );
}

END_NAMESPACE_DRUID
