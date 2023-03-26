
/// @file SimNode2Test.cc
/// @brief SimNode2Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "druid.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

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
	    PrimType gate_type,
	    int vals[]);

  /// @brief val の書き込み読み出しテスト
  /// @param[in] node 対象のノード
  /// @param[in] val 書き込む値
  void
  test_val(SimNode* node,
	   PackedVal val);

};

BEGIN_NONAMESPACE

// val を初期化する．
void
init_val(SimNode* node,
	 PackedVal val)
{
  node->set_val(val);
}

END_NONAMESPACE

// @brief 入力ノードのテストを行う．
void
SimNodeTest::test_input()
{
  SimNode* node = SimNode::new_input(0);

  // val の書き込み読み出しテスト
  init_val(node, PV_ALL1);

  test_val(node, PV_ALL1);
  test_val(node, 0x5555555555555555UL);
  test_val(node, 0xaaaaaaaaaaaaaaaaUL);
  test_val(node, PV_ALL0);

  delete node;
}

// @brief 論理ノードのテストを行う．
// @param[in] ni 入力数
// @param[in] gate_type ゲートの種類
// @param[in] vals 真理値表ベクタ
void
SimNodeTest::test_gate(int ni,
		       PrimType gate_type,
		       int vals[])
{
  int np = 1 << ni;
  vector<SimNode*> inputs(ni);
  for (int i = 0; i < ni; ++ i) {
    inputs[i] = SimNode::new_input(i);
  }
  SimNode* node = SimNode::new_gate(ni, gate_type, inputs);

  // val の書き込み読み出しテスト
  init_val(node, PV_ALL1);

  test_val(node, PV_ALL1);
  test_val(node, 0x5555555555555555UL);
  test_val(node, 0xaaaaaaaaaaaaaaaaUL);
  test_val(node, PV_ALL0);

  // _calc_val() のテスト
  init_val(node, PV_ALL0);
  for (int i = 0; i < ni; ++ i) {
    init_val(inputs[i], PV_ALL0);
  }

  for (int p = 0; p < np; ++ p) {
    for (int i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	inputs[i]->set_val(PV_ALL1);
      }
      else {
	inputs[i]->set_val(PV_ALL0);
      }
    }
    PackedVal val = node->_calc_val();
    if ( vals[p] ) {
      EXPECT_EQ( PV_ALL1, val );
    }
    else {
      EXPECT_EQ( PV_ALL0, val );
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
	  inputs[i]->set_val(PV_ALL1);
	}
	else {
	  inputs[i]->set_val(PV_ALL0);
	}
      }
      PackedVal val = node->_calc_gobs(ipos);
      int q = p ^ (1 << ipos);
      if ( vals[p] != vals[q] ) {
	EXPECT_EQ( PV_ALL1, val );
      }
      else {
	EXPECT_EQ( PV_ALL0, val );
      }
    }
  }

  for (int i = 0; i < ni; ++ i) {
    delete inputs[i];
  }
  delete node;
}

// @brief val の書き込み読み出しテスト
// @param[in] node 対象のノード
// @param[in] val 書き込む値
void
SimNodeTest::test_val(SimNode* node,
		      PackedVal val)
{
  node->set_val(val);
  EXPECT_EQ( val, node->val() );
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

  test_gate(1, PrimType::Buff, vals);
}

TEST_F(SimNodeTest, NOT)
{
  int vals[] = {
    1,
    0,
  };

  test_gate(1, PrimType::Not, vals);
}

TEST_F(SimNodeTest, AND2)
{
  int vals[] = {
    0,
    0,
    0,
    1,
  };

  test_gate(2, PrimType::And, vals);
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

  test_gate(3, PrimType::And, vals);
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

  test_gate(4, PrimType::And, vals);
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

  test_gate(5, PrimType::And, vals);
}

TEST_F(SimNodeTest, NAND2)
{
  int vals[] = {
    1,
    1,
    1,
    0,
  };

  test_gate(2, PrimType::Nand, vals);
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

  test_gate(3, PrimType::Nand, vals);
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

  test_gate(4, PrimType::Nand, vals);
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

  test_gate(5, PrimType::Nand, vals);
}

TEST_F(SimNodeTest, OR2)
{
  int vals[] = {
    0,
    1,
    1,
    1,
  };

  test_gate(2, PrimType::Or, vals);
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

  test_gate(3, PrimType::Or, vals);
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

  test_gate(4, PrimType::Or, vals);
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

  test_gate(5, PrimType::Or, vals);
}

TEST_F(SimNodeTest, NOR2)
{
  int vals[] = {
    1,
    0,
    0,
    0,
  };

  test_gate(2, PrimType::Nor, vals);
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

  test_gate(3, PrimType::Nor, vals);
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

  test_gate(4, PrimType::Nor, vals);
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

  test_gate(5, PrimType::Nor, vals);
}

TEST_F(SimNodeTest, XOR2)
{
  int vals[] = {
    0,
    1,
    1,
    0,
  };

  test_gate(2, PrimType::Xor, vals);
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

  test_gate(3, PrimType::Xor, vals);
}

TEST_F(SimNodeTest, XNOR2)
{
  int vals[] = {
    1,
    0,
    0,
    1,
  };

  test_gate(2, PrimType::Xnor, vals);
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

  test_gate(3, PrimType::Xnor, vals);
}

END_NAMESPACE_DRUID_FSIM