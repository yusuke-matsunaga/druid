
/// @file SimNodeTest.cc
/// @brief SimNodeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "druid.h"
#include "SimNode.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID_FSIM2

class SimNodeTest :
  public ::testing::Test
{
public:

  /// @brief 入力ノードのテストを行う．
  void
  test_input();

  /// @brief 論理ノードのテストを行う．
  /// @param[in] ni 入力数
  /// @param[in] gate_type ゲートの種類
  /// @param[in] vals 真理値表ベクタ
  void
  test_gate(int ni,
	    GateType gate_type,
	    int vals[]);

  /// @brief hval の書き込み読み出しテスト
  /// @param[in] node 対象のノード
  /// @param[in] val 書き込む値
  void
  test_hval(SimNode* node,
	    PackedVal val);

  /// @brief gval の書き込み読み出しテスト
  /// @param[in] node 対象のノード
  /// @param[in] val 書き込む値
  void
  test_gval(SimNode* node,
	    PackedVal val);

  /// @brief fval の書き込み読み出しテスト
  /// @param[in] node 対象のノード
  /// @param[in] val 書き込む値
  void
  test_fval(SimNode* node,
	    PackedVal val);

};

BEGIN_NONAMESPACE

// gval/fval を初期化する．
void
init_val(SimNode* node,
	 PackedVal val)
{
  node->set_hval(val);
  node->set_gval(val);
  node->set_fval(val);
}

END_NONAMESPACE

// @brief 入力ノードのテストを行う．
void
SimNodeTest::test_input()
{
  SimNode* node = SimNode::new_input(0);

  // hval の書き込み読み出しテスト
  init_val(node, PV_ALL0);

  test_hval(node, PV_ALL1);
  test_hval(node, 0x5555555555555555UL);
  test_hval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_hval(node, PV_ALL0);

  // gval の書き込み読み出しテスト
  init_val(node, PV_ALL0);

  test_gval(node, PV_ALL1);
  test_gval(node, 0x5555555555555555UL);
  test_gval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_gval(node, PV_ALL0);

  // fval の書き込み読み出しテスト
  init_val(node, PV_ALL1);

  test_fval(node, PV_ALL1);
  test_fval(node, 0x5555555555555555UL);
  test_fval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_fval(node, PV_ALL0);

  delete node;
}

// @brief 論理ノードのテストを行う．
// @param[in] ni 入力数
// @param[in] gate_type ゲートの種類
// @param[in] vals 真理値表ベクタ
void
SimNodeTest::test_gate(int ni,
		       GateType gate_type,
		       int vals[])
{
  vector<SimNode*> inputs(ni);
  for (int i = 0; i < ni; ++ i) {
    inputs[i] = SimNode::new_input(i);
  }
  SimNode* node = SimNode::new_node(ni, gate_type, inputs);

  int np = 1 << ni;

  // hval の書き込み読み出しテスト
  init_val(node, PV_ALL0);

  test_hval(node, PV_ALL1);
  test_hval(node, 0x5555555555555555UL);
  test_hval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_hval(node, PV_ALL0);

  // gval の書き込み読み出しテスト
  init_val(node, PV_ALL0);

  test_gval(node, PV_ALL1);
  test_gval(node, 0x5555555555555555UL);
  test_gval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_gval(node, PV_ALL0);

  // fval の書き込み読み出しテスト
  init_val(node, PV_ALL1);

  test_fval(node, PV_ALL1);
  test_fval(node, 0x5555555555555555UL);
  test_fval(node, 0xaaaaaaaaaaaaaaaaUL);
  test_fval(node, PV_ALL0);

  // _calc_hval() のテスト
  // ここで書き込む値に対して意味はない．
  init_val(node, PV_ALL0);
  for (int i = 0; i < ni; ++ i) {
    init_val(inputs[i], PV_ALL0);
  }

  for (int p = 0; p < np; ++ p) {
    for (int i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	inputs[i]->set_hval(PV_ALL1);
      }
      else {
	inputs[i]->set_hval(PV_ALL0);
      }
    }
    PackedVal val = node->_calc_hval();
    if ( vals[p] ) {
      EXPECT_EQ( val, PV_ALL1 );
    }
    else {
      EXPECT_EQ( val, PV_ALL0 );
    }
  }

  // _calc_gval() のテスト
  // ここで書き込む値に対して意味はない．
  init_val(node, PV_ALL0);
  for (int i = 0; i < ni; ++ i) {
    init_val(inputs[i], PV_ALL0);
  }

  for (int p = 0; p < np; ++ p) {
    for (int i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	inputs[i]->set_gval(PV_ALL1);
      }
      else {
	inputs[i]->set_gval(PV_ALL0);
      }
    }
    PackedVal val = node->_calc_gval();
    if ( vals[p] ) {
      EXPECT_EQ( val, PV_ALL1 );
    }
    else {
      EXPECT_EQ( val, PV_ALL0 );
    }
  }

  // _calc_fval() のテスト
  // ここで書き込む値に対して意味はない．
  init_val(node, PV_ALL1);
  for (int i = 0; i < ni; ++ i) {
    init_val(inputs[i], PV_ALL1);
  }

  for (int p = 0; p < np; ++ p) {
    for (int i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	inputs[i]->set_fval(PV_ALL1);
      }
      else {
	inputs[i]->set_fval(PV_ALL0);
      }
    }
    PackedVal val = node->_calc_fval();
    if ( vals[p] ) {
      EXPECT_EQ( val, PV_ALL1 );
    }
    else {
      EXPECT_EQ( val, PV_ALL0 );
    }
  }

  // calc_gobs() のテスト
  for (int ipos = 0; ipos < ni; ++ ipos) {
    // ここで書き込む値に対して意味はない．
    init_val(node, PV_ALL0);
    for (int i = 0; i < ni; ++ i) {
      init_val(inputs[i], PV_ALL0);
    }

    for (int p = 0; p < np; ++ p) {
      for (int i = 0; i < ni; ++ i) {
	if ( p & (1 << i) ) {
	  inputs[i]->set_gval(PV_ALL1);
	}
	else {
	  inputs[i]->set_gval(PV_ALL0);
	}
      }
      PackedVal val = node->calc_gobs(ipos);
      int q = p ^ (1 << ipos);
      if ( vals[p] != vals[q] ) {
	EXPECT_EQ( val, PV_ALL1 );
      }
      else {
	EXPECT_EQ( val, PV_ALL0 );
      }
    }
  }

  for (int i = 0; i < ni; ++ i) {
    delete inputs[i];
  }
  delete node;
}

// @brief hval の書き込み読み出しテスト
// @param[in] node 対象のノード
// @param[in] val 書き込む値
void
SimNodeTest::test_hval(SimNode* node,
		       PackedVal val)
{
  node->set_hval(val);
  EXPECT_EQ( node->hval(), val );
}

// @brief gval の書き込み読み出しテスト
// @param[in] node 対象のノード
// @param[in] val 書き込む値
void
SimNodeTest::test_gval(SimNode* node,
		       PackedVal val)
{
  node->set_gval(val);
  EXPECT_EQ( node->gval(), val );
}

// @brief fval の書き込み読み出しテスト
// @param[in] node 対象のノード
// @param[in] val 書き込む値
void
SimNodeTest::test_fval(SimNode* node,
		       PackedVal val)
{
  node->set_fval(val);
  EXPECT_EQ( node->fval(), val );
}

TEST_F(SimNodeTest, INPUT)
{
  test_input();
}

TEST_F(SimNodeTest, BUFF)
{
  int vals[] = {
    0,
    1,
  };

  test_gate(1, GateType::Buff, vals);
}

TEST_F(SimNodeTest, NOT)
{
  int vals[] = {
    1,
    0,
  };

  test_gate(1, GateType::Not, vals);
}

TEST_F(SimNodeTest, AND2)
{
  int vals[] = {
    0,
    0,
    0,
    1,
  };

  test_gate(2, GateType::And, vals);
}

TEST_F(SimNodeTest, AND3)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
  };

  test_gate(3, GateType::And, vals);
}

TEST_F(SimNodeTest, AND4)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
  };

  test_gate(4, GateType::And, vals);
}

TEST_F(SimNodeTest, AND5)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
  };

  test_gate(5, GateType::And, vals);
}

TEST_F(SimNodeTest, NAND2)
{
  int vals[] = {
    1,
    1,
    1,
    0,
  };

  test_gate(2, GateType::Nand, vals);
}

TEST_F(SimNodeTest, NAND3)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
  };

  test_gate(3, GateType::Nand, vals);
}

TEST_F(SimNodeTest, NAND4)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
  };

  test_gate(4, GateType::Nand, vals);
}

TEST_F(SimNodeTest, NAND5)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
  };

  test_gate(5, GateType::Nand, vals);
}

TEST_F(SimNodeTest, OR2)
{
  int vals[] = {
    0,
    1,
    1,
    1,
  };

  test_gate(2, GateType::Or, vals);
}

TEST_F(SimNodeTest, OR3)
{
  int vals[] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
  };

  test_gate(3, GateType::Or, vals);
}

TEST_F(SimNodeTest, OR4)
{
  int vals[] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
  };

  test_gate(4, GateType::Or, vals);
}

TEST_F(SimNodeTest, OR5)
{
  int vals[] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
  };

  test_gate(5, GateType::Or, vals);
}

TEST_F(SimNodeTest, NOR2)
{
  int vals[] = {
    1,
    0,
    0,
    0,
  };

  test_gate(2, GateType::Nor, vals);
}

TEST_F(SimNodeTest, NOR3)
{
  int vals[] = {
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

  test_gate(3, GateType::Nor, vals);
}

TEST_F(SimNodeTest, NOR4)
{
  int vals[] = {
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

  test_gate(4, GateType::Nor, vals);
}

TEST_F(SimNodeTest, NOR5)
{
  int vals[] = {
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
  };

  test_gate(5, GateType::Nor, vals);
}

TEST_F(SimNodeTest, XOR2)
{
  int vals[] = {
    0,
    1,
    1,
    0,
  };

  test_gate(2, GateType::Xor, vals);
}

TEST_F(SimNodeTest, XOR3)
{
  int vals[] = {
    0,
    1,
    1,
    0,
    1,
    0,
    0,
    1,
  };

  test_gate(3, GateType::Xor, vals);
}

TEST_F(SimNodeTest, XNOR2)
{
  int vals[] = {
    1,
    0,
    0,
    1,
  };

  test_gate(2, GateType::Xnor, vals);
}

TEST_F(SimNodeTest, XNOR3)
{
  int vals[] = {
    1,
    0,
    0,
    1,
    0,
    1,
    1,
    0,
  };

  test_gate(3, GateType::Xnor, vals);
}

END_NAMESPACE_DRUID_FSIM2
