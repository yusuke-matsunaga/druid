
/// @file SimNode2Test.cc
/// @brief SimNode2Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "druid.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimNodeTest :
  public ::testing::Test
{
public:

  /// @brief 論理ノードのテストを行う．
  void
  test_gate(
    SizeType ni,        ///< [in] 入力数
    PrimType gate_type, ///< [in] ゲートの種類
    int vals[]          ///< [in] 真理値表ベクタ
  );

};

// @brief 論理ノードのテストを行う．
void
SimNodeTest::test_gate(
  SizeType ni,
  PrimType gate_type,
  int vals[]
)
{
  std::vector<SizeType> inputs(ni);
  std::vector<SimNode*> node_list;
  for (SizeType i = 0; i < ni; ++ i) {
    auto node = SimNode::new_input(i);
    node_list.push_back(node);
    inputs[i] = node->id();
  }
  auto node = SimNode::new_gate(ni, gate_type, 1, inputs);
  node_list.push_back(node);

  std::vector<PackedVal> val_array(ni + 1);

  // _calc_val() のテスト
  SizeType np = 1 << ni;
  for (int p = 0; p < np; ++ p) {
    for (int i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	val_array[i] = PV_ALL1;
      }
      else {
	val_array[i] = PV_ALL0;
      }
    }
    auto val = node->_calc_val(val_array);
    if ( vals[p] ) {
      EXPECT_EQ( PV_ALL1, val );
    }
    else {
      EXPECT_EQ( PV_ALL0, val );
    }
  }

  // calc_gobs() のテスト
  for (int ipos = 0; ipos < ni; ++ ipos) {
    for (int p = 0; p < np; ++ p) {
      for (int i = 0; i < ni; ++ i) {
	if ( p & (1 << i) ) {
	  val_array[i] = PV_ALL1;
	}
	else {
	  val_array[i] = PV_ALL0;
	}
      }
      auto val = node->_calc_gobs(val_array, ipos);
      int q = p ^ (1 << ipos);
      if ( vals[p] != vals[q] ) {
	EXPECT_EQ( PV_ALL1, val );
      }
      else {
	EXPECT_EQ( PV_ALL0, val );
      }
    }
  }

  for ( auto node: node_list ) {
    delete node;
  }
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
