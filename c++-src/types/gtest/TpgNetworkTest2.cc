
/// @file TpgNetworkTest2.cc
/// @brief TpgNetworkTest2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "NetBuilder.h"
#include "NodeRep.h"
#include "GateRep.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkTest2 :
public ::testing::Test
{
public:

  // 外部入力を用意する．
  void
  make_inputs(
    SizeType input_num
  );

  // プリミティブ型のゲートを一つもつネットワークを作る．
  TpgNetwork
  make_primitive(
    PrimType prim_type,
    SizeType input_num
  );

  // 複合型のゲートを一つもつネットワークを作る．
  TpgNetwork
  make_cplx(
    const Expr& expr
  );

  // BnModel
  BnModel mModel;

  // 入力のリスト
  std::vector<BnNode> mInputList;

};


// 外部入力を用意する．
void
TpgNetworkTest2::make_inputs(
  SizeType input_num
)
{
  mInputList.clear();
  mInputList.reserve(input_num);
  for ( SizeType i = 0; i < input_num; ++ i ) {
    std::ostringstream buf;
    buf << "input" << i;
    auto name = buf.str();
    auto node = mModel.new_input(name);
    mInputList.push_back(node);
  }
}

// プリミティブ型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgNetworkTest2::make_primitive(
  PrimType prim_type,
  SizeType input_num
)
{
  make_inputs(input_num);
  auto node = mModel.new_primitive(prim_type, mInputList);
  mModel.new_output(node, "output");
  return TpgNetwork::from_bn(mModel, FaultType::StuckAt);
}

// 複合型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgNetworkTest2::make_cplx(
  const Expr& expr
)
{
  auto input_num = expr.input_size();
  make_inputs(input_num);
  auto node = mModel.new_expr(expr, mInputList);
  mModel.new_output(node, "output");
  return TpgNetwork::from_bn(mModel, FaultType::StuckAt);
}

TEST_F(TpgNetworkTest2, xor2)
{
  // 2入力XORゲート1つからなるネットワークを作る．
  auto tpg_network = make_primitive(PrimType::Xor, 2);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: Xor( Node#0 Node#1 )\n"
    "Node#3: OUTPUT#0 = Node#2\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#3\n"
    "  Node#3\n"
    "  Node#2\n"
    "  Node#0\n"
    "  Node#1\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#3\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "\n"
    "PPO#0: output: Node#3\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Input#0: Node#2[0](Node#0)\n"
    "  Input#1: Node#2[1](Node#1)\n"
    "  Stem Faults: G#2:O:SA0 G#2:O:SA1\n"
    "  Branch Faults: G#2:I0:SA0 G#2:I0:SA1 G#2:I1:SA0 G#2:I1:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#3\n"
    "  Input#0: Node#3[0](Node#2)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#2:I0:SA0\n"
    "  G#2:I0:SA1\n"
    "  G#2:I1:SA0\n"
    "  G#2:I1:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, xor3)
{
  // 3入力XORゲート1つからなるネットワークを作る．
  auto tpg_network = make_primitive(PrimType::Xor, 3);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Xor( Node#0 Node#1 Node#2 )\n"
    "Node#4: OUTPUT#0 = Node#3\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#4\n"
    "  Node#4\n"
    "  Node#3\n"
    "  Node#0\n"
    "  Node#1\n"
    "  Node#2\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#4\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "\n"
    "PPO#0: output: Node#4\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#3\n"
    "  Input#0: Node#3[0](Node#0)\n"
    "  Input#1: Node#3[1](Node#1)\n"
    "  Input#2: Node#3[2](Node#2)\n"
    "  Stem Faults: G#3:O:SA0 G#3:O:SA1\n"
    "  Branch Faults: G#3:I0:SA0 G#3:I0:SA1 G#3:I1:SA0 G#3:I1:SA1 G#3:I2:SA0 G#3:I2:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#4\n"
    "  Input#0: Node#4[0](Node#3)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#3:I0:SA0\n"
    "  G#3:I0:SA1\n"
    "  G#3:I1:SA0\n"
    "  G#3:I1:SA1\n"
    "  G#3:I2:SA0\n"
    "  G#3:I2:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, xor4)
{
  // 4入力XORゲート1つからなるネットワークを作る．
  auto tpg_network = make_primitive(PrimType::Xor, 4);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: INPUT#3\n"
    "Node#4: Xor( Node#0 Node#1 Node#2 Node#3 )\n"
    "Node#5: OUTPUT#0 = Node#4\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#5\n"
    "  Node#5\n"
    "  Node#4\n"
    "  Node#0\n"
    "  Node#1\n"
    "  Node#2\n"
    "  Node#3\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#5\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "PPI#3: input3: Node#3\n"
    "\n"
    "PPO#0: output: Node#5\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#3\n"
    "  Stem Faults: I#3:O:SA0 I#3:O:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#4\n"
    "  Input#0: Node#4[0](Node#0)\n"
    "  Input#1: Node#4[1](Node#1)\n"
    "  Input#2: Node#4[2](Node#2)\n"
    "  Input#3: Node#4[3](Node#3)\n"
    "  Stem Faults: G#4:O:SA0 G#4:O:SA1\n"
    "  Branch Faults: G#4:I0:SA0 G#4:I0:SA1 G#4:I1:SA0 G#4:I1:SA1 G#4:I2:SA0 G#4:I2:SA1 G#4:I3:SA0 G#4:I3:SA1\n"
    "\n"
    "GATE#5:\n"
    "  Output: Node#5\n"
    "  Input#0: Node#5[0](Node#4)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#4:I0:SA0\n"
    "  G#4:I0:SA1\n"
    "  G#4:I1:SA0\n"
    "  G#4:I1:SA1\n"
    "  G#4:I2:SA0\n"
    "  G#4:I2:SA1\n"
    "  G#4:I3:SA0\n"
    "  G#4:I3:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, aoi22)
{
  // AOI221つからなるネットワークを作る．
  auto a1_lit = Expr::positive_literal(0);
  auto a2_lit = Expr::positive_literal(1);
  auto b1_lit = Expr::positive_literal(2);
  auto b2_lit = Expr::positive_literal(3);
  auto aoi22_expr = ~((a1_lit & a2_lit) | (b1_lit & b2_lit));
  auto tpg_network = make_cplx(aoi22_expr);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
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
    "  Node#11\n"
    "  Node#10\n"
    "  Node#8\n"
    "  Node#9\n"
    "  Node#6\n"
    "  Node#7\n"
    "  Node#3\n"
    "  Node#2\n"
    "  Node#4\n"
    "  Node#5\n"
    "  Node#1\n"
    "  Node#0\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#11\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "PPI#3: input3: Node#3\n"
    "\n"
    "PPO#0: output: Node#11\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#3\n"
    "  Stem Faults: I#3:O:SA0 I#3:O:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#10\n"
    "  Input#0: Node#4[0](Node#0)\n"
    "  Input#1: Node#5[0](Node#1)\n"
    "  Input#2: Node#6[0](Node#2)\n"
    "  Input#3: Node#7[0](Node#3)\n"
    "  Stem Faults: G#4:O:SA0 G#4:O:SA1\n"
    "  Branch Faults: G#4:I0:SA0 G#4:I0:SA1 G#4:I1:SA0 G#4:I1:SA1 G#4:I2:SA0 G#4:I2:SA1 G#4:I3:SA0 G#4:I3:SA1\n"
    "\n"
    "GATE#5:\n"
    "  Output: Node#11\n"
    "  Input#0: Node#11[0](Node#10)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#4:I0:SA0\n"
    "  G#4:I0:SA1\n"
    "  G#4:I1:SA0\n"
    "  G#4:I1:SA1\n"
    "  G#4:I2:SA0\n"
    "  G#4:I2:SA1\n"
    "  G#4:I3:SA0\n"
    "  G#4:I3:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, oai21)
{
  // OAI21 1つからなるネットワークを作る．
  auto a1_lit = Expr::positive_literal(0);
  auto a2_lit = Expr::positive_literal(1);
  auto b1_lit = Expr::positive_literal(2);
  auto oai21_expr = ~((a1_lit | a2_lit) & b1_lit);
  auto tpg_network = make_cplx(oai21_expr);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
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
    "  Node#8\n"
    "  Node#7\n"
    "  Node#6\n"
    "  Node#5\n"
    "  Node#2\n"
    "  Node#3\n"
    "  Node#4\n"
    "  Node#1\n"
    "  Node#0\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#8\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "\n"
    "PPO#0: output: Node#8\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#7\n"
    "  Input#0: Node#3[0](Node#0)\n"
    "  Input#1: Node#4[0](Node#1)\n"
    "  Input#2: Node#5[0](Node#2)\n"
    "  Stem Faults: G#3:O:SA0 G#3:O:SA1\n"
    "  Branch Faults: G#3:I0:SA0 G#3:I0:SA1 G#3:I1:SA0 G#3:I1:SA1 G#3:I2:SA0 G#3:I2:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#8\n"
    "  Input#0: Node#8[0](Node#7)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#3:I0:SA0\n"
    "  G#3:I0:SA1\n"
    "  G#3:I1:SA0\n"
    "  G#3:I1:SA1\n"
    "  G#3:I2:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, cplx1)
{
  // OAI21 1つからなるネットワークを作る．
  auto a1_lit = Expr::positive_literal(0);
  auto a2_lit = Expr::positive_literal(1);
  auto b1_lit = Expr::positive_literal(2);
  auto cplx1_expr = (a1_lit | a2_lit) & b1_lit;
  auto tpg_network = make_cplx(cplx1_expr);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
    "Node#0: INPUT#0\n"
    "Node#1: INPUT#1\n"
    "Node#2: INPUT#2\n"
    "Node#3: Or( Node#0 Node#1 )\n"
    "Node#4: And( Node#3 Node#2 )\n"
    "Node#5: OUTPUT#0 = Node#4\n"
    "\n"
    "FFR#0\n"
    "  ROOT: Node#5\n"
    "  Node#5\n"
    "  Node#4\n"
    "  Node#3\n"
    "  Node#2\n"
    "  Node#0\n"
    "  Node#1\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#5\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "\n"
    "PPO#0: output: Node#5\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#4\n"
    "  Input#0: Node#3[0](Node#0)\n"
    "  Input#1: Node#3[1](Node#1)\n"
    "  Input#2: Node#4[1](Node#2)\n"
    "  Stem Faults: G#3:O:SA0 G#3:O:SA1\n"
    "  Branch Faults: G#3:I0:SA0 G#3:I0:SA1 G#3:I1:SA0 G#3:I1:SA1 G#3:I2:SA0 G#3:I2:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#5\n"
    "  Input#0: Node#5[0](Node#4)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#3:I0:SA0\n"
    "  G#3:I0:SA1\n"
    "  G#3:I1:SA0\n"
    "  G#3:I1:SA1\n"
    "  G#3:I2:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

TEST_F(TpgNetworkTest2, cplx2)
{
  // OAI21 1つからなるネットワークを作る．
  auto a1_lit = Expr::positive_literal(0);
  auto a2_lit = Expr::positive_literal(1);
  auto b1_lit = Expr::positive_literal(2);
  auto cplx2_expr = (a1_lit & b1_lit ) | (a2_lit & b1_lit);
  auto tpg_network = make_cplx(cplx2_expr);
  ASSERT_TRUE( tpg_network.is_valid() );

  std::ostringstream buf;
  tpg_network.print(buf);

  std::string ref_str =
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
    "  Node#3\n"
    "  Node#2\n"
    "FFR#1\n"
    "  ROOT: Node#7\n"
    "  INPUT#0: Node#3\n"
    "  Node#7\n"
    "  Node#6\n"
    "  Node#4\n"
    "  Node#5\n"
    "  Node#1\n"
    "  Node#0\n"
    "\n"
    "MFFC#0\n"
    "  ROOT: Node#7\n"
    "  FFR#1\n"
    "  FFR#0\n"
    "\n"
    "PPI#0: input0: Node#0\n"
    "PPI#1: input1: Node#1\n"
    "PPI#2: input2: Node#2\n"
    "\n"
    "PPO#0: output: Node#7\n"
    "\n"
    "GATE#0:\n"
    "  Output: Node#0\n"
    "  Stem Faults: I#0:O:SA0 I#0:O:SA1\n"
    "\n"
    "GATE#1:\n"
    "  Output: Node#1\n"
    "  Stem Faults: I#1:O:SA0 I#1:O:SA1\n"
    "\n"
    "GATE#2:\n"
    "  Output: Node#2\n"
    "  Stem Faults: I#2:O:SA0 I#2:O:SA1\n"
    "\n"
    "GATE#3:\n"
    "  Output: Node#6\n"
    "  Input#0: Node#4[0](Node#0)\n"
    "  Input#1: Node#5[0](Node#1)\n"
    "  Input#2: Node#3[0](Node#2)\n"
    "  Stem Faults: G#3:O:SA0 G#3:O:SA1\n"
    "  Branch Faults: G#3:I0:SA0 G#3:I0:SA1 G#3:I1:SA0 G#3:I1:SA1 G#3:I2:SA0 G#3:I2:SA1\n"
    "\n"
    "GATE#4:\n"
    "  Output: Node#7\n"
    "  Input#0: Node#7[0](Node#6)\n"
    "  Branch Faults: O#0:I0:SA0 O#0:I0:SA1\n"
    "\n"
    "Representative Faults\n"
    "  G#3:I0:SA0\n"
    "  G#3:I0:SA1\n"
    "  G#3:I1:SA0\n"
    "  G#3:I1:SA1\n"
    "  G#3:I2:SA1\n"
    "  O#0:I0:SA0\n"
    "  O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, buf.str() );
}

END_NAMESPACE_DRUID
