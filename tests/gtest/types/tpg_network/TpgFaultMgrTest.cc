
/// @file TpgFaultMgrTest.cc
/// @brief TpgFaultMgrTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgFaultMgr.h"
#include "TpgFault.h"
#include "TpgNetwork.h"
#include "ym/BnModel.h"
#include "ym/BnNode.h"
#include "ym/BnSeq.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

void
print_faults(
  TpgFaultMgr& fmgr,
  ostream& s
)
{
  for ( auto f: fmgr.rep_fault_list() ) {
    s << f.id() << ": " << f << endl;
    s << "  ExCond:";
    for ( auto nv: f.excitation_condition() ) {
      s << " " << nv;
    }
    s << endl;
    s << "  PropCond:";
    for ( auto nv: f.ffr_propagate_condition() ) {
      s << " " << nv;
    }
    s << endl;
  }
  s << "# of rep faults: " << fmgr.rep_fault_list().size() << endl;
}

TEST(TpgNetworkTest, and2)
{
  // 2入力ANDゲート1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto b = model.new_input("b");
  auto node = model.new_primitive(PrimType::And, {a, b});
  model.new_output(node, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "7: Gate#0:I0:SA1\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "9: Gate#0:I1:SA1\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "10: x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, or2)
{
  // 2入力ANDゲート1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto b = model.new_input("b");
  auto node = model.new_primitive(PrimType::Or, {a, b});
  model.new_output(node, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "6: Gate#0:I0:SA0\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "8: Gate#0:I1:SA0\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "10: x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, and_or2)
{
  // 2入力ANDゲート1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto b = model.new_input("b");
  auto c = model.new_input("c");
  auto d = model.new_input("d");
  auto node1 = model.new_primitive(PrimType::And, {a, b});
  auto node2 = model.new_primitive(PrimType::And, {c, d});
  auto node3 = model.new_primitive(PrimType::Or, {node1, node2});
  model.new_output(node3, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "11: Gate#0:I0:SA1\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1 Node#5@1 = 0\n"
    "13: Gate#0:I1:SA1\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0 Node#5@1 = 0\n"
    "17: Gate#1:I0:SA1\n"
    "  ExCond: Node#2@1 = 0 Node#3@1 = 1\n"
    "  PropCond: Node#2@1 = 0 Node#3@1 = 1 Node#4@1 = 0\n"
    "19: Gate#1:I1:SA1\n"
    "  ExCond: Node#2@1 = 1 Node#3@1 = 0\n"
    "  PropCond: Node#2@1 = 1 Node#3@1 = 0 Node#4@1 = 0\n"
    "22: Gate#2:I0:SA0\n"
    "  ExCond: Node#4@1 = 1 Node#5@1 = 0\n"
    "  PropCond: Node#4@1 = 1 Node#5@1 = 0\n"
    "24: Gate#2:I1:SA0\n"
    "  ExCond: Node#4@1 = 0 Node#5@1 = 1\n"
    "  PropCond: Node#4@1 = 0 Node#5@1 = 1\n"
    "26: x:I0:SA0\n"
    "  ExCond: Node#6@1 = 1\n"
    "  PropCond: Node#6@1 = 1\n"
    "27: x:I0:SA1\n"
    "  ExCond: Node#6@1 = 0\n"
    "  PropCond: Node#6@1 = 0\n"
    "# of rep faults: 8\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, xor2)
{
  // 2入力XORゲート1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto b = model.new_input("b");
  auto node = model.new_primitive(PrimType::Xor, {a, b});
  model.new_output(node, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "6: Gate#0:I0:SA0\n"
    "  ExCond: Node#0@1 = 1\n"
    "  PropCond: Node#0@1 = 1\n"
    "7: Gate#0:I0:SA1\n"
    "  ExCond: Node#0@1 = 0\n"
    "  PropCond: Node#0@1 = 0\n"
    "8: Gate#0:I1:SA0\n"
    "  ExCond: Node#1@1 = 1\n"
    "  PropCond: Node#1@1 = 1\n"
    "9: Gate#0:I1:SA1\n"
    "  ExCond: Node#1@1 = 0\n"
    "  PropCond: Node#1@1 = 0\n"
    "10: x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 6\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, dff1)
{
  // D-FF 1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto clk = model.new_input("clk");
  auto dff = model.new_dff(' ', "dff1");
  auto dff_out = dff.data_output();
  model.set_data_src(dff, a);
  model.set_clock(dff, clk);
  model.new_output(dff_out, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string exp_str =
    "4: x:I0:SA0\n"
    "  ExCond: Node#1@1 = 1\n"
    "  PropCond: Node#1@1 = 1\n"
    "5: x:I0:SA1\n"
    "  ExCond: Node#1@1 = 0\n"
    "  PropCond: Node#1@1 = 0\n"
    "6: dff1.input:I0:SA0\n"
    "  ExCond: Node#0@1 = 1\n"
    "  PropCond: Node#0@1 = 1\n"
    "7: dff1.input:I0:SA1\n"
    "  ExCond: Node#0@1 = 0\n"
    "  PropCond: Node#0@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( exp_str, buf.str() );
}

TEST(TpgNetworkTest, dff2)
{
  // D-FF 1つからなるネットワークを作る．
  BnModel model;
  auto a = model.new_input("a");
  auto clk = model.new_input("clk");
  auto dff = model.new_dff(' ', "dff1");
  auto dff_out = dff.data_output();
  model.set_data_src(dff, a);
  model.set_clock(dff, clk);
  model.new_output(dff_out, "x");

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::TransitionDelay);

  ostringstream buf;
  print_faults(fmgr, buf);

  string exp_str =
    "4: x:I0:RISE\n"
    "  ExCond: Node#1@0 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#1@0 = 0 Node#1@1 = 1\n"
    "5: x:I0:FALL\n"
    "  ExCond: Node#1@0 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#1@0 = 1 Node#1@1 = 0\n"
    "6: dff1.input:I0:RISE\n"
    "  ExCond: Node#0@0 = 0 Node#0@1 = 1\n"
    "  PropCond: Node#0@0 = 0 Node#0@1 = 1\n"
    "7: dff1.input:I0:FALL\n"
    "  ExCond: Node#0@0 = 1 Node#0@1 = 0\n"
    "  PropCond: Node#0@0 = 1 Node#0@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( exp_str, buf.str() );
}

END_NAMESPACE_DRUID
