
/// @file NodeRepTest.cc
/// @brief NodeRepTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "types/Val3.h"
#include "NodeRep.h"

// テストの方針
//
// NodeRep の全てのメンバ関数をテストする．
// 継承クラスによってはメンバ関数の呼び出しが std::logic_error
// 例外を送出する場合がある．
//
// 生成する NodeRep の種類は以下の通り
// - NodeRep::new_input(SizeType input_id)
//
// - NodeRep::new_dff_output(SizeType input_id,
//                            SizeType dff_id)
//
// - NodeRep::new_output(SizeType output_id,
//                        const NodeRep* fanin)
//
// - NodeRep::new_dff_input(SizeType output_id,
//                           SizeType dff_id,
//                           const NodeRep* fanin)
//
// - NodeRep::new_logic(PrimType gate_type,
//                       const std::vector<const NodeRep*>& fanin_list)

BEGIN_NAMESPACE_DRUID

class NodeRepTest :
public ::testing::Test
{
public:

  void
  SetUp()
  {
    for ( auto i: { 0, 1, 2, 3, 4 } ) {
      mInputNodeList[i] = NodeRep::new_input(i, i);
    }
  }

  // 入力ノードに関するテスト
  void
  input_test(
    NodeRep* node,
    const std::string& type_name,
    SizeType input_id
  )
  {
    if ( type_name == "input" ) {
      EXPECT_TRUE ( node->is_primary_input() );
      EXPECT_FALSE( node->is_dff_output() );
      EXPECT_TRUE ( node->is_ppi() );
      EXPECT_EQ( input_id, node->input_id() );
    }
    else if ( type_name == "dff_output" ) {
      EXPECT_FALSE( node->is_primary_input() );
      EXPECT_TRUE ( node->is_dff_output() );
      EXPECT_TRUE ( node->is_ppi() );
      EXPECT_EQ( input_id, node->input_id() );
    }
    else {
      EXPECT_FALSE( node->is_dff_output() );
      EXPECT_FALSE( node->is_primary_input() );
      EXPECT_FALSE( node->is_ppi() );
      EXPECT_THROW( node->input_id(),
		    std::logic_error );
    }
  }

  // 出力ノードに関するテスト
  void
  output_test(
    NodeRep* node,
    const std::string& type_name,
    SizeType output_id,
    SizeType output_id2
  )
  {
    if ( type_name == "output" ) {
      EXPECT_TRUE ( node->is_primary_output() );
      EXPECT_FALSE( node->is_dff_input() );
      EXPECT_TRUE ( node->is_ppo() );
      EXPECT_EQ( output_id, node->output_id() );
      EXPECT_NO_THROW( node->set_output_id2(output_id2) );
      EXPECT_EQ( output_id2, node->output_id2() );
    }
    else if ( type_name == "dff_input" ) {
      EXPECT_FALSE( node->is_primary_output() );
      EXPECT_TRUE ( node->is_dff_input() );
      EXPECT_TRUE ( node->is_ppo() );
      EXPECT_EQ( output_id, node->output_id() );
      EXPECT_NO_THROW( node->set_output_id2(output_id2) );
      EXPECT_EQ( output_id2, node->output_id2() );
    }
    else {
      EXPECT_FALSE( node->is_primary_output() );
      EXPECT_FALSE( node->is_dff_input() );
      EXPECT_FALSE( node->is_ppo() );
      EXPECT_THROW( node->output_id(),
		    std::logic_error );
      EXPECT_THROW( node->set_output_id2(output_id2),
		    std::logic_error );
      EXPECT_THROW( node->output_id2(),
		    std::logic_error );
      EXPECT_THROW( node->output_id2(),
		    std::logic_error );
    }
  }

  // DFFの入出力ノードに関するテスト
  void
  dff_test(
    NodeRep* node,
    const std::string& type_name,
    SizeType dff_id,
    const NodeRep* alt_node
  )
  {
    if ( type_name == "dff_input" || type_name == "dff_output" ) {
      EXPECT_EQ( dff_id, node->dff_id() );
      EXPECT_NO_THROW( node->set_alt_node(alt_node) );
      EXPECT_EQ( alt_node, node->alt_node() );
    }
    else {
      EXPECT_THROW( node->dff_id(),
		    std::logic_error );
      EXPECT_THROW( node->set_alt_node(alt_node),
		    std::logic_error );
      EXPECT_THROW( node->alt_node(),
		    std::logic_error );
    }
  }

  // 論理タイプのノードに関するテスト
  void
  logic_test(
    NodeRep* node,
    const std::string& type_name,
    PrimType gate_type,
    const std::vector<const NodeRep*>& fanin_list
  )
  {
    if ( type_name == "logic" ) {
      EXPECT_TRUE ( node->is_logic() );
    }
    else {
      EXPECT_FALSE( node->is_logic() );
    }
    EXPECT_EQ( gate_type, node->gate_type() );
    auto ni = fanin_list.size();
    EXPECT_EQ( ni, node->fanin_num() );
    for ( SizeType i = 0; i < ni; ++ i ) {
      EXPECT_EQ( fanin_list[i], node->fanin(i) );
    }
    EXPECT_EQ( fanin_list, node->fanin_list() );
    EXPECT_THROW( node->fanin(ni),
		  std::out_of_range );
  }

  // cval, nval, coval, noval のテスト
  void
  cval_test(
    NodeRep* node,
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

  // ファンインのリストを作る．
  std::vector<const NodeRep*>
  make_fanin_list(
    SizeType ni
  )
  {
    std::vector<const NodeRep*> fanin_list(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      fanin_list[i] = mInputNodeList[i];
    }
    return fanin_list;
  }

  // 入力ノード
  NodeRep* mInputNodeList[5];

};


TEST_F(NodeRepTest, input)
{
  SizeType id = 99;
  SizeType iid = 15;
  auto node = NodeRep::new_input(id, iid);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "input";
  input_test(node, type_name, iid);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, PrimType::None, {});
}

TEST_F(NodeRepTest, dff_output)
{
  SizeType id = 99;
  SizeType iid = 15;
  SizeType dff_id = 3;
  auto node = NodeRep::new_dff_output(id, iid, dff_id);
  ASSERT_TRUE( node != nullptr );

  auto alt_node = NodeRep::new_dff_input(id, 0, dff_id, nullptr);
  ASSERT_TRUE( alt_node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "dff_output";
  input_test(node, type_name, iid);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, dff_id, alt_node);
  logic_test(node, type_name, PrimType::None, {});
}

TEST_F(NodeRepTest, output)
{
  SizeType id = 99;
  SizeType oid = 23;
  SizeType oid2 = 41;
  auto fanin = mInputNodeList[0];
  auto node = NodeRep::new_output(id, oid, fanin);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "output";
  input_test(node, type_name, 0);
  output_test(node, type_name, oid, oid2);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, PrimType::Buff, {fanin});
}

TEST_F(NodeRepTest, dff_input)
{
  SizeType id = 99;
  SizeType oid = 23;
  SizeType oid2 = 41;
  SizeType dff_id = 4;
  auto fanin = mInputNodeList[0];
  auto node = NodeRep::new_dff_input(id, oid, dff_id, fanin);
  ASSERT_TRUE( node != nullptr );
  auto alt_node = NodeRep::new_dff_output(id, 0, dff_id);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "dff_input";
  input_test(node, type_name, 0);
  output_test(node, type_name, oid, oid2);
  dff_test(node, type_name, dff_id, alt_node);
  logic_test(node, type_name, PrimType::Buff, {fanin});
}

TEST_F(NodeRepTest, CONST0)
{
  SizeType id = 99;
  auto gate_type = PrimType::C0;
  std::vector<const NodeRep*> fanin_list{};
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);
}

TEST_F(NodeRepTest, CONST1)
{
  SizeType id = 99;
  auto gate_type = PrimType::C1;
  std::vector<const NodeRep*> fanin_list{};
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);
}

TEST_F(NodeRepTest, Buff)
{
  SizeType id = 99;
  auto gate_type = PrimType::Buff;
  auto fanin_list = make_fanin_list(1);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(NodeRepTest, Not)
{
  SizeType id = 99;
  auto gate_type = PrimType::Not;
  auto fanin_list = make_fanin_list(1);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(NodeRepTest, AND2)
{
  SizeType id = 99;
  auto gate_type = PrimType::And;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, AND3)
{
  SizeType id = 99;
  auto gate_type = PrimType::And;
  auto fanin_list = make_fanin_list(3);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, AND4)
{
  SizeType id = 99;
  auto gate_type = PrimType::And;
  auto fanin_list = make_fanin_list(4);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, AND5)
{
  SizeType id = 99;
  auto gate_type = PrimType::And;
  auto fanin_list = make_fanin_list(5);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, NAND2)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nand;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, NAND3)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nand;
  auto fanin_list = make_fanin_list(3);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, NAND4)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nand;
  auto fanin_list = make_fanin_list(4);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, NAND5)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nand;
  auto fanin_list = make_fanin_list(5);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_0, Val3::_1, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, OR2)
{
  SizeType id = 99;
  auto gate_type = PrimType::Or;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, OR3)
{
  SizeType id = 99;
  auto gate_type = PrimType::Or;
  auto fanin_list = make_fanin_list(3);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, OR4)
{
  SizeType id = 99;
  auto gate_type = PrimType::Or;
  auto fanin_list = make_fanin_list(4);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, OR5)
{
  SizeType id = 99;
  auto gate_type = PrimType::Or;
  auto fanin_list = make_fanin_list(5);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_1, Val3::_0);
}

TEST_F(NodeRepTest, NOR2)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nor;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, NOR3)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nor;
  auto fanin_list = make_fanin_list(3);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, NOR4)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nor;
  auto fanin_list = make_fanin_list(4);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, NOR5)
{
  SizeType id = 99;
  auto gate_type = PrimType::Nor;
  auto fanin_list = make_fanin_list(5);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_1, Val3::_0, Val3::_0, Val3::_1);
}

TEST_F(NodeRepTest, XOR2)
{
  SizeType id = 99;
  auto gate_type = PrimType::Xor;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

TEST_F(NodeRepTest, XNOR2)
{
  SizeType id = 99;
  auto gate_type = PrimType::Xnor;
  auto fanin_list = make_fanin_list(2);
  auto node = NodeRep::new_logic(id, gate_type, fanin_list);
  ASSERT_TRUE( node != nullptr );

  EXPECT_EQ( id, node->id() );
  std::string type_name = "logic";
  input_test(node, type_name, 0);
  output_test(node, type_name, 0, 0);
  dff_test(node, type_name, 0, nullptr);
  logic_test(node, type_name, gate_type, fanin_list);

  cval_test(node, Val3::_X, Val3::_X, Val3::_X, Val3::_X);
}

END_NAMESPACE_DRUID
