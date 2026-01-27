
/// @file TpgNodeTest.cc
/// @brief TpgNodeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "types/Val3.h"
#include "types/FaultType.h"
#include "NetworkRep.h"
#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

class TpgNodeTest :
public ::testing::Test
{
public:

  void
  SetUp();

  // NodeRep* から TpgNode を作る．
  TpgNode
  to_node(
    NodeRep* node
  )
  {
    return TpgNode(mNetwork, node->id());
  }

  // 外部入力のテスト
  void
  input_test(
    const TpgNode& node,
    SizeType input_id
  );

  // DFF出力のテスト
  void
  dff_output_test(
    const TpgNode& node,
    SizeType input_id,
    SizeType dff_id,
    const TpgNode& alt_node
  );

  // 疑似入力のテスト
  void
  ppi_test(
    const TpgNode& node,
    SizeType input_id
  );

  // 外部出力のテスト
  void
  output_test(
    const TpgNode& node,
    SizeType output_id,
    SizeType output_id2
  );

  // DFF入力のテスト
  void
  dff_input_test(
    const TpgNode& node,
    SizeType output_id,
    SizeType output_id2,
    SizeType dff_id,
    const TpgNode& alt_node
  );

  // 疑似出力のテスト
  void
  ppo_test(
    const TpgNode& node,
    SizeType output_id,
    SizeType output_id2
  );

  // 論理タイプのテスト
  void
  logic_test(
    const TpgNode& node,
    PrimType gate_type,
    SizeType ni
  );

  // 制御値のテスト
  void
  cval_test(
    const TpgNode& node,
    Val3 exp_cval,
    Val3 exp_nval,
    Val3 exp_coval,
    Val3 exp_noval
  );

  // 親のネットワーク
  std::shared_ptr<NetworkRep> mNetwork;

  // 入力ノード
  NodeRep* mInputNodeList[5];

};

void
TpgNodeTest::SetUp()
{
  mNetwork = std::shared_ptr<NetworkRep>{new NetworkRep(FaultType::StuckAt)};
  for ( auto i: { 0, 1, 2, 3, 4 } ) {
    std::ostringstream buf;
    buf << "input" << i;
    mInputNodeList[i] = mNetwork->make_input_node(buf.str());
  }
}

// 外部入力のテスト
void
TpgNodeTest::input_test(
  const TpgNode& node,
  SizeType input_id
)
{
  EXPECT_TRUE ( node.is_primary_input() );
  EXPECT_FALSE( node.is_dff_output() );
  EXPECT_THROW( node.dff_id(),
		std::logic_error );
  EXPECT_THROW( node.alt_node(),
		std::logic_error );
  ppi_test(node, input_id );
}

// DFF出力のテスト
void
TpgNodeTest::dff_output_test(
  const TpgNode& node,
  SizeType input_id,
  SizeType dff_id,
  const TpgNode& alt_node
)
{
  EXPECT_FALSE( node.is_primary_input() );
  EXPECT_TRUE ( node.is_dff_output() );
  EXPECT_EQ( dff_id, node.dff_id() );
  EXPECT_EQ( alt_node, node.alt_node() );
  ppi_test(node, input_id);
}

// 疑似入力のテスト
void
TpgNodeTest::ppi_test(
  const TpgNode& node,
  SizeType input_id
)
{
  EXPECT_TRUE ( node.is_ppi() );
  EXPECT_FALSE( node.is_primary_output() );
  EXPECT_FALSE( node.is_dff_input() );
  EXPECT_FALSE( node.is_ppo() );
  EXPECT_EQ( input_id, node.input_id() );
  EXPECT_THROW( node.output_id(),
		std::logic_error );
  EXPECT_THROW( node.output_id2(),
		std::logic_error );
  EXPECT_FALSE( node.is_logic() );
  EXPECT_EQ( PrimType::None, node.gate_type() );
  ASSERT_EQ( 0, node.fanin_num() );
  EXPECT_THROW( node.fanin(0),
		std::out_of_range );
  auto fanin_list = node.fanin_list();
  ASSERT_EQ( 0, fanin_list.size() );
}

// 外部出力のテスト
void
TpgNodeTest::output_test(
  const TpgNode& node,
  SizeType output_id,
  SizeType output_id2
)
{
  EXPECT_TRUE ( node.is_primary_output() );
  EXPECT_FALSE( node.is_dff_input() );
  EXPECT_THROW( node.dff_id(),
		std::logic_error );
  EXPECT_THROW( node.alt_node(),
		std::logic_error );
  ppo_test(node, output_id, output_id2);
}

// DFF入力のテスト
void
TpgNodeTest::dff_input_test(
  const TpgNode& node,
  SizeType output_id,
  SizeType output_id2,
  SizeType dff_id,
  const TpgNode& alt_node
)
{
  EXPECT_FALSE( node.is_primary_output() );
  EXPECT_TRUE ( node.is_dff_input() );
  EXPECT_EQ( dff_id, node.dff_id() );
  EXPECT_EQ( alt_node, node.alt_node() );
  ppo_test(node, output_id, output_id2);
}

// 疑似出力のテスト
void
TpgNodeTest::ppo_test(
  const TpgNode& node,
  SizeType output_id,
  SizeType output_id2
)
{
  EXPECT_FALSE( node.is_primary_input() );
  EXPECT_FALSE( node.is_dff_output() );
  EXPECT_FALSE( node.is_ppi() );
  EXPECT_TRUE ( node.is_ppo() );
  EXPECT_THROW( node.input_id(),
		std::logic_error );
  EXPECT_EQ( output_id, node.output_id() );
  EXPECT_EQ( output_id2, node.output_id2() );
  EXPECT_FALSE( node.is_logic() );
  EXPECT_EQ( PrimType::Buff, node.gate_type() );
  ASSERT_EQ( 1, node.fanin_num() );
  EXPECT_EQ( to_node(mInputNodeList[0]), node.fanin(0) );
  EXPECT_THROW( node.fanin(1),
		std::out_of_range );
  EXPECT_EQ( 0, node.fanout_num() );
  EXPECT_THROW( node.fanout(0),
		std::out_of_range );
  auto fanout_list = node.fanout_list();
  EXPECT_TRUE( fanout_list.empty() );

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

// 論理タイプのテスト
void
TpgNodeTest::logic_test(
  const TpgNode& node,
  PrimType gate_type,
  SizeType ni
)
{
  EXPECT_FALSE( node.is_primary_input() );
  EXPECT_FALSE( node.is_dff_output() );
  EXPECT_FALSE( node.is_ppi() );
  EXPECT_FALSE( node.is_primary_output() );
  EXPECT_FALSE( node.is_dff_input() );
  EXPECT_FALSE( node.is_ppo() );
  EXPECT_THROW( node.input_id(),
		std::logic_error );
  EXPECT_THROW( node.output_id(),
		std::logic_error );
  EXPECT_THROW( node.output_id2(),
		std::logic_error );
  EXPECT_THROW( node.dff_id(),
		std::logic_error );
  EXPECT_THROW( node.alt_node(),
		std::logic_error );
  EXPECT_TRUE( node.is_logic() );
  EXPECT_EQ( gate_type, node.gate_type() );
  ASSERT_EQ( ni, node.fanin_num() );
  for ( SizeType i = 0; i < ni; ++ i ) {
    EXPECT_EQ( to_node(mInputNodeList[i]), node.fanin(i) );
  }
  EXPECT_THROW( node.fanin(ni),
		std::out_of_range );
  auto fanin_list = node.fanin_list();
  ASSERT_EQ( ni, fanin_list.size() );
  for ( SizeType i = 0; i < ni; ++ i ) {
    EXPECT_EQ( to_node(mInputNodeList[i]), fanin_list[i] );
  }
}

// cval, nval, coval, noval のテスト
void
TpgNodeTest::cval_test(
  const TpgNode& node,
  Val3 exp_cval,
  Val3 exp_nval,
  Val3 exp_coval,
  Val3 exp_noval
)
{
  EXPECT_EQ( exp_cval, node.cval() );
  EXPECT_EQ( exp_nval, node.nval() );
  EXPECT_EQ( exp_coval, node.coval() );
  EXPECT_EQ( exp_noval, node.noval() );
}


TEST_F(TpgNodeTest, input)
{
  SizeType iid = mNetwork->ppi_num();
  auto node = to_node(mNetwork->make_input_node("input"));

  ASSERT_TRUE( node.is_valid() );
  input_test(node, iid);
}

TEST_F(TpgNodeTest, dff_output)
{
  SizeType iid = mNetwork->ppi_num();
  SizeType dff_id = mNetwork->dff_num();
  auto node = to_node(mNetwork->make_dff_output_node("dff_output"));

  auto fanin = mInputNodeList[0];
  auto alt_node = to_node(mNetwork->make_dff_input_node(dff_id, "dff_input", fanin));
  ASSERT_TRUE( node.is_valid() );
  dff_output_test(node, iid, dff_id, alt_node);
}

TEST_F(TpgNodeTest, output)
{
  SizeType oid = mNetwork->ppo_num();
  auto node_rep = mNetwork->make_output_node("output", mInputNodeList[0]);
  SizeType id2 = 99;
  node_rep->set_output_id2(id2);
  auto node = to_node(node_rep);

  ASSERT_TRUE( node.is_valid() );
  output_test(node, oid, id2);
}

TEST_F(TpgNodeTest, dff_input)
{
  SizeType iid = mNetwork->ppi_num();
  SizeType dff_id = mNetwork->dff_num();
  auto alt_node = to_node(mNetwork->make_dff_output_node("dff_output"));

  SizeType oid = mNetwork->ppo_num();
  auto fanin = mInputNodeList[0];
  auto node_rep = mNetwork->make_dff_input_node(dff_id, "dff_input", fanin);
  SizeType id2 = 99;
  node_rep->set_output_id2(id2);
  auto node = to_node(node_rep);
  ASSERT_TRUE( node.is_valid() );
  dff_input_test(node, oid, id2, dff_id, alt_node);
}

TEST_F(TpgNodeTest, CONST0)
{
  auto node = to_node(mNetwork->make_prim_node(PrimType::C0, {}));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::C0, 0);
}

TEST_F(TpgNodeTest, CONST1)
{
  auto node = to_node(mNetwork->make_prim_node(PrimType::C1, {}));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::C1, 0);
}

TEST_F(TpgNodeTest, BUFF)
{
  auto node = to_node(mNetwork->make_prim_node(PrimType::Buff, {mInputNodeList[0]}));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::Buff, 1);
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, NOT)
{
  auto node = to_node(mNetwork->make_prim_node(PrimType::Not, {mInputNodeList[0]}));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::Not, 1);
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, AND2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = to_node(mNetwork->make_prim_node(PrimType::And, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::And, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND3)
{
  int ni = 3;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = to_node(mNetwork->make_prim_node(PrimType::And, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::And, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND4)
{
  int ni = 4;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = to_node(mNetwork->make_prim_node(PrimType::And, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::And, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND5)
{
  int ni = 5;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = to_node(mNetwork->make_prim_node(PrimType::And, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, PrimType::And, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NAND2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nand;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND3)
{
  int ni = 3;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nand;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND4)
{
  int ni = 4;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nand;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND5)
{
  int ni = 5;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nand;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Or;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR3)
{
  int ni = 3;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Or;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR4)
{
  int ni = 4;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Or;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR5)
{
  int ni = 5;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Or;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NOR2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR3)
{
  int ni = 3;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR4)
{
  int ni = 4;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR5)
{
  int ni = 5;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Nor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, XOR2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Xor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, XNOR2)
{
  int ni = 2;
  std::vector<const NodeRep*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto gate_type = PrimType::Xnor;
  auto node = to_node(mNetwork->make_prim_node(gate_type, fanin_list));

  ASSERT_TRUE( node.is_valid() );
  logic_test(node, gate_type, ni);
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

END_NAMESPACE_DRUID
