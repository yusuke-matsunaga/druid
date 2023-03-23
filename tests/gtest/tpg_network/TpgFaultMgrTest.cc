
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
#include "ym/BnModifier.h"
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
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_b = mod.new_input_port("b");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto b = port_b.bit(0);
  auto x = port_x.bit(0);
  auto node = mod.new_logic_primitive("and1", PrimType::And, {a, b});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "7: and1:I0:SA1\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "9: and1:I1:SA1\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "10: *x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: *x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, or2)
{
  // 2入力ANDゲート1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_b = mod.new_input_port("b");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto b = port_b.bit(0);
  auto x = port_x.bit(0);
  auto node = mod.new_logic_primitive("or1", PrimType::Or, {a, b});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "6: or1:I0:SA0\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "8: or1:I1:SA0\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "10: *x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: *x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 4\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, and_or2)
{
  // 2入力ANDゲート1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_b = mod.new_input_port("b");
  auto port_c = mod.new_input_port("c");
  auto port_d = mod.new_input_port("d");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto b = port_b.bit(0);
  auto c = port_c.bit(0);
  auto d = port_d.bit(0);
  auto x = port_x.bit(0);
  auto node1 = mod.new_logic_primitive("and1", PrimType::And, {a, b});
  auto node2 = mod.new_logic_primitive("and2", PrimType::And, {c, d});
  auto node3 = mod.new_logic_primitive("or1", PrimType::Or, {node1, node2});
  mod.set_output_src(x, node3);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "11: and1:I0:SA1\n"
    "  ExCond: Node#0@1 = 0 Node#1@1 = 1\n"
    "  PropCond: Node#0@1 = 0 Node#1@1 = 1 Node#5@1 = 0\n"
    "13: and1:I1:SA1\n"
    "  ExCond: Node#0@1 = 1 Node#1@1 = 0\n"
    "  PropCond: Node#0@1 = 1 Node#1@1 = 0 Node#5@1 = 0\n"
    "17: and2:I0:SA1\n"
    "  ExCond: Node#2@1 = 0 Node#3@1 = 1\n"
    "  PropCond: Node#2@1 = 0 Node#3@1 = 1 Node#4@1 = 0\n"
    "19: and2:I1:SA1\n"
    "  ExCond: Node#2@1 = 1 Node#3@1 = 0\n"
    "  PropCond: Node#2@1 = 1 Node#3@1 = 0 Node#4@1 = 0\n"
    "22: or1:I0:SA0\n"
    "  ExCond: Node#4@1 = 1 Node#5@1 = 0\n"
    "  PropCond: Node#4@1 = 1 Node#5@1 = 0\n"
    "24: or1:I1:SA0\n"
    "  ExCond: Node#4@1 = 0 Node#5@1 = 1\n"
    "  PropCond: Node#4@1 = 0 Node#5@1 = 1\n"
    "26: *x:I0:SA0\n"
    "  ExCond: Node#6@1 = 1\n"
    "  PropCond: Node#6@1 = 1\n"
    "27: *x:I0:SA1\n"
    "  ExCond: Node#6@1 = 0\n"
    "  PropCond: Node#6@1 = 0\n"
    "# of rep faults: 8\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, xor2)
{
  // 2入力XORゲート1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_b = mod.new_input_port("b");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto b = port_b.bit(0);
  auto x = port_x.bit(0);
  auto node = mod.new_logic_primitive("xor1", PrimType::Xor, {a, b});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);

  ostringstream buf;
  print_faults(fmgr, buf);

  string ref_str =
    "6: xor1:I0:SA0\n"
    "  ExCond: Node#0@1 = 1\n"
    "  PropCond: Node#0@1 = 1\n"
    "7: xor1:I0:SA1\n"
    "  ExCond: Node#0@1 = 0\n"
    "  PropCond: Node#0@1 = 0\n"
    "8: xor1:I1:SA0\n"
    "  ExCond: Node#1@1 = 1\n"
    "  PropCond: Node#1@1 = 1\n"
    "9: xor1:I1:SA1\n"
    "  ExCond: Node#1@1 = 0\n"
    "  PropCond: Node#1@1 = 0\n"
    "10: *x:I0:SA0\n"
    "  ExCond: Node#2@1 = 1\n"
    "  PropCond: Node#2@1 = 1\n"
    "11: *x:I0:SA1\n"
    "  ExCond: Node#2@1 = 0\n"
    "  PropCond: Node#2@1 = 0\n"
    "# of rep faults: 6\n";

  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, dff1)
{
  // D-FF 1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_clk = mod.new_input_port("clk");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto clk = port_clk.bit(0);
  auto x = port_x.bit(0);
  auto dff = mod.new_dff("dff1");
  auto dff_in = dff.data_in();
  auto dff_out = dff.data_out();
  auto dff_clk = dff.clock();
  mod.set_output_src(dff_in, a);
  mod.set_output_src(dff_clk, clk);
  mod.set_output_src(x, dff_out);

}

END_NAMESPACE_DRUID
