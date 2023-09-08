
/// @file xor_test.cc
/// @brief xor_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgNetwork.h"
#include "ym/BnModifier.h"
#include "ym/BnNetwork.h"


BEGIN_NAMESPACE_DRUID

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

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: Xor( Node#0 Node#1 )\n"
    "Node#3: OUTPUT#0 = Node#2\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#3\n"
    "  INPUT#0: Node#0\n"
    "  INPUT#1: Node#1\n"
    "  Node#3\n"
    "  Node#2\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#3\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a: Node#0\n"
    "PPI#1: b: Node#1\n"
    "\n"
    "PPO#0: *x: Node#3\n"
    "\n"
    "GATE#0: xor1\n"
    "  Output: Node#2\n"
    "  Input#0: Node#2[0]\n"
    "  Input#1: Node#2[1]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, xor3)
{
  // 3入力XORゲート1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a = mod.new_input_port("a");
  auto port_b = mod.new_input_port("b");
  auto port_c = mod.new_input_port("c");
  auto port_x = mod.new_output_port("x");
  auto a = port_a.bit(0);
  auto b = port_b.bit(0);
  auto c = port_c.bit(0);
  auto x = port_x.bit(0);
  auto node = mod.new_logic_primitive("xor1", PrimType::Xor, {a, b, c});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Xor( Node#0 Node#1 Node#2 )\n"
    "Node#4: OUTPUT#0 = Node#3\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#4\n"
    "  INPUT#0: Node#0\n"
    "  INPUT#1: Node#1\n"
    "  INPUT#2: Node#2\n"
    "  Node#4\n"
    "  Node#3\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#4\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a: Node#0\n"
    "PPI#1: b: Node#1\n"
    "PPI#2: c: Node#2\n"
    "\n"
    "PPO#0: *x: Node#4\n"
    "\n"
    "GATE#0: xor1\n"
    "  Output: Node#3\n"
    "  Input#0: Node#3[0]\n"
    "  Input#1: Node#3[1]\n"
    "  Input#2: Node#3[2]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, xor4)
{
  // 4入力XORゲート1つからなるネットワークを作る．
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
  auto node = mod.new_logic_primitive("xor1", PrimType::Xor, {a, b, c, d});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: INPUT#3\n"
    "Node#4: Xor( Node#0 Node#1 Node#2 Node#3 )\n"
    "Node#5: OUTPUT#0 = Node#4\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#5\n"
    "  INPUT#0: Node#0\n"
    "  INPUT#1: Node#1\n"
    "  INPUT#2: Node#2\n"
    "  INPUT#3: Node#3\n"
    "  Node#5\n"
    "  Node#4\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#5\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a: Node#0\n"
    "PPI#1: b: Node#1\n"
    "PPI#2: c: Node#2\n"
    "PPI#3: d: Node#3\n"
    "\n"
    "PPO#0: *x: Node#5\n"
    "\n"
    "GATE#0: xor1\n"
    "  Output: Node#4\n"
    "  Input#0: Node#4[0]\n"
    "  Input#1: Node#4[1]\n"
    "  Input#2: Node#4[2]\n"
    "  Input#3: Node#4[3]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, aoi22)
{
  // AOI221つからなるネットワークを作る．
  BnModifier mod;
  auto port_a1 = mod.new_input_port("a1");
  auto port_a2 = mod.new_input_port("a2");
  auto port_b1 = mod.new_input_port("b1");
  auto port_b2 = mod.new_input_port("b2");
  auto port_x = mod.new_output_port("x");
  auto a1 = port_a1.bit(0);
  auto a2 = port_a2.bit(0);
  auto b1 = port_b1.bit(0);
  auto b2 = port_b2.bit(0);
  auto x = port_x.bit(0);
  auto a1_lit = Expr::make_posi_literal(0);
  auto a2_lit = Expr::make_posi_literal(1);
  auto b1_lit = Expr::make_posi_literal(2);
  auto b2_lit = Expr::make_posi_literal(3);
  auto aoi22_expr = ~((a1_lit & a2_lit) | (b1_lit & b2_lit));
  auto node = mod.new_logic_expr("aoi22", aoi22_expr, {a1, a2, b1, b2});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: INPUT#3\n"
    "Node#4: Not( Node#0 )\n"
    "Node#5: Not( Node#1 )\n"
    "Node#6: Not( Node#2 )\n"
    "Node#7: Not( Node#3 )\n"
    "Node#8: Or( Node#4 Node#5 )\n"
    "Node#9: Or( Node#6 Node#7 )\n"
    "Node#10: And( Node#8 Node#9 )\n"
    "Node#11: OUTPUT#0 = Node#10\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#11\n"
    "  INPUT#0: Node#3\n"
    "  INPUT#1: Node#2\n"
    "  INPUT#2: Node#1\n"
    "  INPUT#3: Node#0\n"
    "  Node#11\n"
    "  Node#10\n"
    "  Node#8\n"
    "  Node#9\n"
    "  Node#6\n"
    "  Node#7\n"
    "  Node#4\n"
    "  Node#5\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#11\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a1: Node#0\n"
    "PPI#1: a2: Node#1\n"
    "PPI#2: b1: Node#2\n"
    "PPI#3: b2: Node#3\n"
    "\n"
    "PPO#0: *x: Node#11\n"
    "\n"
    "GATE#0: aoi22\n"
    "  Output: Node#10\n"
    "  Input#0: Node#4[0]\n"
    "  Input#1: Node#5[0]\n"
    "  Input#2: Node#6[0]\n"
    "  Input#3: Node#7[0]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, oai21)
{
  // OAI21 1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a1 = mod.new_input_port("a1");
  auto port_a2 = mod.new_input_port("a2");
  auto port_b1 = mod.new_input_port("b1");
  auto port_x = mod.new_output_port("x");
  auto a1 = port_a1.bit(0);
  auto a2 = port_a2.bit(0);
  auto b1 = port_b1.bit(0);
  auto x = port_x.bit(0);
  auto a1_lit = Expr::make_posi_literal(0);
  auto a2_lit = Expr::make_posi_literal(1);
  auto b1_lit = Expr::make_posi_literal(2);
  auto oai21_expr = ~((a1_lit | a2_lit) & b1_lit);
  auto node = mod.new_logic_expr("oai21", oai21_expr, {a1, a2, b1});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Not( Node#0 )\n"
    "Node#4: Not( Node#1 )\n"
    "Node#5: Not( Node#2 )\n"
    "Node#6: And( Node#3 Node#4 )\n"
    "Node#7: Or( Node#6 Node#5 )\n"
    "Node#8: OUTPUT#0 = Node#7\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#8\n"
    "  INPUT#0: Node#2\n"
    "  INPUT#1: Node#1\n"
    "  INPUT#2: Node#0\n"
    "  Node#8\n"
    "  Node#7\n"
    "  Node#6\n"
    "  Node#5\n"
    "  Node#3\n"
    "  Node#4\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#8\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a1: Node#0\n"
    "PPI#1: a2: Node#1\n"
    "PPI#2: b1: Node#2\n"
    "\n"
    "PPO#0: *x: Node#8\n"
    "\n"
    "GATE#0: oai21\n"
    "  Output: Node#7\n"
    "  Input#0: Node#3[0]\n"
    "  Input#1: Node#4[0]\n"
    "  Input#2: Node#5[0]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, cplx1)
{
  // OAI21 1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a1 = mod.new_input_port("a1");
  auto port_a2 = mod.new_input_port("a2");
  auto port_b1 = mod.new_input_port("b1");
  auto port_x = mod.new_output_port("x");
  auto a1 = port_a1.bit(0);
  auto a2 = port_a2.bit(0);
  auto b1 = port_b1.bit(0);
  auto x = port_x.bit(0);
  auto a1_lit = Expr::make_posi_literal(0);
  auto a2_lit = Expr::make_posi_literal(1);
  auto b1_lit = Expr::make_posi_literal(2);
  auto cplx1_expr = (a1_lit | a2_lit) & b1_lit;
  auto node = mod.new_logic_expr("cplx1", cplx1_expr, {a1, a2, b1});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Or( Node#0 Node#1 )\n"
    "Node#4: And( Node#3 Node#2 )\n"
    "Node#5: OUTPUT#0 = Node#4\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#5\n"
    "  INPUT#0: Node#2\n"
    "  INPUT#1: Node#0\n"
    "  INPUT#2: Node#1\n"
    "  Node#5\n"
    "  Node#4\n"
    "  Node#3\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#5\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a1: Node#0\n"
    "PPI#1: a2: Node#1\n"
    "PPI#2: b1: Node#2\n"
    "\n"
    "PPO#0: *x: Node#5\n"
    "\n"
    "GATE#0: cplx1\n"
    "  Output: Node#4\n"
    "  Input#0: Node#3[0]\n"
    "  Input#1: Node#3[1]\n"
    "  Input#2: Node#4[1]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST(TpgNetworkTest, cplx2)
{
  // OAI21 1つからなるネットワークを作る．
  BnModifier mod;
  auto port_a1 = mod.new_input_port("a1");
  auto port_a2 = mod.new_input_port("a2");
  auto port_b1 = mod.new_input_port("b1");
  auto port_x = mod.new_output_port("x");
  auto a1 = port_a1.bit(0);
  auto a2 = port_a2.bit(0);
  auto b1 = port_b1.bit(0);
  auto x = port_x.bit(0);
  auto a1_lit = Expr::make_posi_literal(0);
  auto a2_lit = Expr::make_posi_literal(1);
  auto b1_lit = Expr::make_posi_literal(2);
  auto cplx2_expr = (a1_lit & b1_lit ) | (a2_lit & b1_lit);
  auto node = mod.new_logic_expr("cplx2", cplx2_expr, {a1, a2, b1});
  mod.set_output_src(x, node);

  BnNetwork bn_net1{mod};
  auto tpg_network = TpgNetwork{bn_net1};

  ostringstream buf;
  tpg_network.print(buf);

  string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Buff( Node#2 )\n"
    "Node#4: And( Node#0 Node#3 )\n"
    "Node#5: And( Node#1 Node#3 )\n"
    "Node#6: Or( Node#4 Node#5 )\n"
    "Node#7: OUTPUT#0 = Node#6\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#3\n"
    "  INPUT#0: Node#2\n"
    "  Node#3\n"
    "FFR#1\n"
    "  ROOT: Node#7\n"
    "  INPUT#0: Node#1\n"
    "  INPUT#1: Node#3\n"
    "  INPUT#2: Node#0\n"
    "  Node#7\n"
    "  Node#6\n"
    "  Node#4\n"
    "  Node#5\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#7\n"
    "  FFR#1\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: a1: Node#0\n"
    "PPI#1: a2: Node#1\n"
    "PPI#2: b1: Node#2\n"
    "\n"
    "PPO#0: *x: Node#7\n"
    "\n"
    "GATE#0: cplx2\n"
    "  Output: Node#6\n"
    "  Input#0: Node#4[0]\n"
    "  Input#1: Node#5[0]\n"
    "  Input#2: Node#3[0]\n"
    "\n";
  EXPECT_EQ( ref_str, buf.str() );
}

END_NAMESPACE_DRUID
