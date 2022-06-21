
/// @file TpgNodeTest.cc
/// @brief TpgNodeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "TpgNode.h"
#include "TpgNetworkImpl.h"
#include "GateType.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

class TpgNodeTest :
public ::testing::Test
{
public:

  // コンストラクタ
  TpgNodeTest();

  // cval, nval, coval, noval のテスト
  void
  cval_test(
    TpgNode* node,
    Val3 exp_cval,
    Val3 exp_nval,
    Val3 exp_coval,
    Val3 exp_noval
  );

  // ファクトリ
  TpgNetworkImpl mNetwork;

  // 入力ノード
  TpgNode* mInputNodeList[5];

};

TpgNodeTest::TpgNodeTest()
{
  for ( auto i: { 0, 1, 2, 3, 4 } ) {
    ostringstream buf;
    buf << "input" << i;
    mInputNodeList[i] = mNetwork.make_input_node(i, buf.str(), 1);
  }
}

// cval, nval, coval, noval のテスト
void
TpgNodeTest::cval_test(
  TpgNode* node,
  Val3 exp_cval,
  Val3 exp_nval,
  Val3 exp_coval,
  Val3 exp_noval
)
{
  EXPECT_EQ( exp_cval, node->cval() );
  EXPECT_EQ( exp_nval, node->nval() );
  EXPECT_EQ( exp_coval, node->coval() );
  EXPECT_EQ( exp_noval, node->noval() );
}


TEST_F(TpgNodeTest, input)
{
  int iid = 20;
  int fanout_num = 0;
  auto node = mNetwork.make_input_node(iid, "input", fanout_num);

  ASSERT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, output)
{
  int oid = 20;
  auto node = mNetwork.make_output_node(oid, "output", mInputNodeList[0]);

  ASSERT_TRUE( node != nullptr );

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, CONST0)
{
  auto node = mNetwork.make_logic(GateType::Const0, {}, 0);

  ASSERT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, CONST1)
{
  auto node = mNetwork.make_logic(GateType::Const1, {}, 0);

  ASSERT_TRUE( node != nullptr );
}

TEST_F(TpgNodeTest, BUFF)
{
  auto node = mNetwork.make_logic(GateType::Buff, {mInputNodeList[0]}, 0);

  ASSERT_TRUE( node != nullptr );

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, NOT)
{
  auto node = mNetwork.make_logic(GateType::Not, {mInputNodeList[0]}, 0);

  ASSERT_TRUE( node != nullptr );

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, AND2)
{
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::And, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::And, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND3)
{
  int ni = 3;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::And, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::And, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND4)
{
  int ni = 4;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::And, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::And, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, AND5)
{
  int ni = 5;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::And, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::And, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NAND2)
{
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nand, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nand, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND3)
{
  int id = 10;
  int ni = 3;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nand, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND4)
{
  int id = 10;
  int ni = 4;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nand, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nand, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NAND5)
{
  int id = 10;
  int ni = 5;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nand, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nand, node->gate_type() );
  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR2)
{
  int id = 10;
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Or, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Or, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR3)
{
  int id = 10;
  int ni = 3;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Or, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Or, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR4)
{
  int id = 10;
  int ni = 4;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Or, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Or, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, OR5)
{
  int id = 10;
  int ni = 5;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Or, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Or, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(TpgNodeTest, NOR2)
{
  int id = 10;
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nor, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR3)
{
  int id = 10;
  int ni = 3;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nor, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR4)
{
  int id = 10;
  int ni = 4;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nor, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, NOR5)
{
  int id = 10;
  int ni = 5;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Nor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Nor, node->gate_type() );
  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(TpgNodeTest, XOR2)
{
  int id = 10;
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Xor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Xor, node->gate_type() );
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(TpgNodeTest, XNOR2)
{
  int id = 10;
  int ni = 2;
  vector<const TpgNode*> fanin_list(ni);
  for ( int i = 0; i < ni; ++ i ) {
    fanin_list[i] = mInputNodeList[i];
  }
  auto node = mNetwork.make_logic(GateType::Xnor, fanin_list, 0);

  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( GateType::Xnor, node->gate_type() );
  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

END_NAMESPACE_DRUID