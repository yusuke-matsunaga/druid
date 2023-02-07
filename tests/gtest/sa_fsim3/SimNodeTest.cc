
/// @file SimNodeTest.cc
/// @brief SimNodeTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
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

  /// @brief fval の書き込み読み出しテスト
  /// @param[in] node 対象のノード
  /// @param[in] val0, val1 書き込む値
  /// @param[in] exp_val0, exp_val1 期待値
  void
  test_val(SimNode* node,
	   PackedVal val0,
	   PackedVal val1,
	   PackedVal exp_val0,
	   PackedVal exp_val1);

  /// @brief 3値の検証を行う．
  /// @param[in] val0 0のワード
  /// @param[in] val1 1のワード
  /// @param[in] exp_val 期待値
  ///
  /// 期待値は 0, 1, 2(X) の3種類
  void
  test_val3(PackedVal val0,
	    PackedVal val1,
	    int exp_val);
};

BEGIN_NONAMESPACE

// gval/fval を初期化する．
void
init_val(SimNode* node,
	 PackedVal val0,
	 PackedVal val1)
{
  node->set_val(PackedVal3(val0, val1));
}

END_NONAMESPACE

// @brief 入力ノードのテストを行う．
void
SimNodeTest::test_input()
{
  SimNode* node = SimNode::new_input(0);

  // val の書き込み読み出しテスト
  init_val(node, PV_ALL1, PV_ALL1);

  test_val(node,
	   PV_ALL1,              PV_ALL0,
	   PV_ALL1,              PV_ALL0);
  test_val(node,
	   0x5555555555555555UL, 0x5555555555555555UL,
	   0x0000000000000000UL, 0x0000000000000000UL);
  test_val(node,
	   0xaaaaaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaaaaUL,
	   0x0000000000000000UL, 0x0000000000000000UL);
  test_val(node,
	   PV_ALL0,              PV_ALL1,
	   PV_ALL0,              PV_ALL1);

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
  vector<SimNode*> inputs(ni);
  for (int i = 0; i < ni; ++ i) {
    inputs[i] = SimNode::new_input(i);
  }
  SimNode* node = SimNode::new_gate(ni, gate_type, inputs);

  // val の書き込み読み出しテスト
  init_val(node, PV_ALL1, PV_ALL1);

  test_val(node,
	   PV_ALL1,              PV_ALL0,
	   PV_ALL1,              PV_ALL0);
  test_val(node,
	   0x5555555555555555UL, 0x5555555555555555UL,
	   0x0000000000000000UL, 0x0000000000000000UL);
  test_val(node,
	   0xaaaaaaaaaaaaaaaaUL, 0xaaaaaaaaaaaaaaaaUL,
	   0x0000000000000000UL, 0x0000000000000000UL);
  test_val(node,
	   PV_ALL0,              PV_ALL1,
	   PV_ALL0,              PV_ALL1);

  int np = 1;
  for (int i = 0; i < ni; ++ i) {
    np *= 3;
  }
  // _calc_val() のテスト
  // ここで書き込む値に対して意味はない．
  init_val(node, PV_ALL1, PV_ALL1);
  for (int i = 0; i < ni; ++ i) {
    init_val(inputs[i], PV_ALL1, PV_ALL1);
  }

  for (int p = 0; p < np; ++ p) {
    int x = p;
    for (int i = 0; i < ni; ++ i) {
      int y = x % 3;
      x /= 3;
      PackedVal val0;
      PackedVal val1;
      switch ( y ) {
      case 0: val0 = PV_ALL1; val1 = PV_ALL0; break;
      case 1: val0 = PV_ALL0; val1 = PV_ALL1; break;
      case 2: val0 = PV_ALL0; val1 = PV_ALL0; break;
      }
      init_val(inputs[i], val0, val1);
    }
    PackedVal3 val = node->_calc_val();
    PackedVal val0 = val.val0();
    PackedVal val1 = val.val1();
    test_val3(val0, val1, vals[p]);
  }

  // calc_gobs() のテスト
  for (int ipos = 0; ipos < ni; ++ ipos) {
    // ここで書き込む値に対して意味はない．
    init_val(node, PV_ALL0, PV_ALL1);
    for (int i = 0; i < ni; ++ i) {
      init_val(inputs[i], PV_ALL0, PV_ALL1);
    }

    vector<int> ivals(ni, 0);
    for ( ; ; ) {
      int p = 0;
      int q = 0;
      int w = 1;
      for (int i = 0; i < ni; ++ i) {
	PackedVal val0;
	PackedVal val1;
	switch ( ivals[i] ) {
	case 0: val0 = PV_ALL1; val1 = PV_ALL0; break;
	case 1: val0 = PV_ALL0; val1 = PV_ALL1; break;
	case 2: val0 = PV_ALL0; val1 = PV_ALL0; break;
	}
	init_val(inputs[i], val0, val1);
	if ( i == ipos ) {
	  p += 0 * w; // じつは効果なし．形式を合わせただけ
	  q += 1 * w;
	}
	else {
	  p += ivals[i] * w;
	  q += ivals[i] * w;
	}
	w *= 3;
      }

      PackedVal val = node->_calc_gobs(ipos);
      if ( vals[q] != vals[p] &&
	   vals[q] != 2 &&
	   vals[p] != 2 ) {
	EXPECT_EQ( PV_ALL1, val );
      }
      else {
	EXPECT_EQ( PV_ALL0, val );
      }

      // ivals を次の値へシフトする．
      for (int i = 0; i < ni; ++ i) {
	++ ivals[i];
	if ( ivals[i] == 3 ) {
	  ivals[i] = 0;
	}
	else {
	  break;
	}
      }
      bool end = true;
      for (int i = 0; i < ni; ++ i) {
	if ( ivals[i] != 0 ) {
	  end = false;
	  break;
	}
      }
      if ( end ) {
	break;
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
// @param[in] val0, val1 書き込む値
// @param[in] exp_val0, exp_val1 期待値
void
SimNodeTest::test_val(SimNode* node,
		      PackedVal val0,
		      PackedVal val1,
		      PackedVal exp_val0,
		      PackedVal exp_val1)
{
  // 書き込んだ値が読み出せるかテストする．
  init_val(node, val0, val1);
  EXPECT_EQ( exp_val0, node->val().val0() );
  EXPECT_EQ( exp_val1, node->val().val1() );
}

// @brief 3値の検証を行う．
// @param[in] val0 0のワード
// @param[in] val1 1のワード
// @param[in] exp_val 期待値
//
// 期待値は 0, 1, 2(X) の3種類
void
SimNodeTest::test_val3(PackedVal val0,
		       PackedVal val1,
		       int exp_val)
{
  switch ( exp_val ) {
  case 0:
    EXPECT_EQ( PV_ALL1, val0 );
    EXPECT_EQ( PV_ALL0, val1 );
    break;

  case 1:
    EXPECT_EQ( PV_ALL0, val0 );
    EXPECT_EQ( PV_ALL1, val1 );
    break;

  case 2:
    EXPECT_EQ( PV_ALL0, val0 );
    EXPECT_EQ( PV_ALL0, val1 );
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
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
    2,
  };

  test_gate(1, PrimType::Buff, vals);
}

TEST_F(SimNodeTest, NOT)
{
  int vals[] = {
    1,
    0,
    2,
  };

  test_gate(1, PrimType::Not, vals);
}

TEST_F(SimNodeTest, AND2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( x == 0 || y == 0 ) {
	val = 0;
      }
      else if ( x == 1 && y == 1 ) {
	val = 1;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::And, vals);
}

TEST_F(SimNodeTest, AND3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 2;
	if ( x == 0 || y == 0 || z == 0 ) {
	  val = 0;
	}
	else if ( x == 1 && y == 1 && z == 1 ) {
	  val = 1;
	}
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::And, vals);
}

TEST_F(SimNodeTest, AND4)
{
  int vals[3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  int val = 2;
	  if ( x == 0 || y == 0 || z == 0 || w == 0 ) {
	    val = 0;
	  }
	  else if ( x == 1 && y == 1 && z == 1 && w == 1 ) {
	    val = 1;
	  }
	  vals[((x * 3 + y) * 3 + z) * 3 + w] = val;
	}
      }
    }
  }

  test_gate(4, PrimType::And, vals);
}

TEST_F(SimNodeTest, AND5)
{
  int vals[3 * 3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  for (int u = 0; u < 3; ++ u) {
	    int val = 2;
	    if ( x == 0 || y == 0 || z == 0 || w == 0 || u == 0 ) {
	      val = 0;
	    }
	    else if ( x == 1 && y == 1 && z == 1 && w == 1 && u == 1 ) {
	      val = 1;
	    }
	    vals[(((x * 3 + y) * 3 + z) * 3 + w) * 3 + u] = val;
	  }
	}
      }
    }
  }

  test_gate(5, PrimType::And, vals);
}

TEST_F(SimNodeTest, NAND2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( x == 0 || y == 0 ) {
	val = 1;
      }
      else if ( x == 1 && y == 1 ) {
	val = 0;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::Nand, vals);
}

TEST_F(SimNodeTest, NAND3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 2;
	if ( x == 0 || y == 0 || z == 0 ) {
	  val = 1;
	}
	else if ( x == 1 && y == 1 && z == 1 ) {
	  val = 0;
	}
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::Nand, vals);
}

TEST_F(SimNodeTest, NAND4)
{
  int vals[3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  int val = 2;
	  if ( x == 0 || y == 0 || z == 0 || w == 0 ) {
	    val = 1;
	  }
	  else if ( x == 1 && y == 1 && z == 1 && w == 1 ) {
	    val = 0;
	  }
	  vals[((x * 3 + y) * 3 + z) * 3 + w] = val;
	}
      }
    }
  }

  test_gate(4, PrimType::Nand, vals);
}

TEST_F(SimNodeTest, NAND5)
{
  int vals[3 * 3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  for (int u = 0; u < 3; ++ u) {
	    int val = 2;
	    if ( x == 0 || y == 0 || z == 0 || w == 0 || u == 0 ) {
	      val = 1;
	    }
	    else if ( x == 1 && y == 1 && z == 1 && w == 1 && u == 1 ) {
	      val = 0;
	    }
	    vals[(((x * 3 + y) * 3 + z) * 3 + w) * 3 + u] = val;
	  }
	}
      }
    }
  }

  test_gate(5, PrimType::Nand, vals);
}

TEST_F(SimNodeTest, OR2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( x == 0 && y == 0 ) {
	val = 0;
      }
      else if ( x == 1 || y == 1 ) {
	val = 1;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::Or, vals);
}

TEST_F(SimNodeTest, OR3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 2;
	if ( x == 0 && y == 0 && z == 0 ) {
	  val = 0;
	}
	else if ( x == 1 || y == 1 || z == 1 ) {
	  val = 1;
	}
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::Or, vals);
}

TEST_F(SimNodeTest, OR4)
{
  int vals[3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  int val = 2;
	  if ( x == 0 && y == 0 && z == 0 && w == 0 ) {
	    val = 0;
	  }
	  else if ( x == 1 || y == 1 || z == 1 || w == 1 ) {
	    val = 1;
	  }
	  vals[((x * 3 + y) * 3 + z) * 3 + w] = val;
	}
      }
    }
  }

  test_gate(4, PrimType::Or, vals);
}

TEST_F(SimNodeTest, OR5)
{
  int vals[3 * 3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  for (int u = 0; u < 3; ++ u) {
	    int val = 2;
	    if ( x == 0 && y == 0 && z == 0 && w == 0 && u == 0 ) {
	      val = 0;
	    }
	    else if ( x == 1 || y == 1 || z == 1 || w == 1 || u == 1 ) {
	      val = 1;
	    }
	    vals[(((x * 3 + y) * 3 + z) * 3 + w) * 3 + u] = val;
	  }
	}
      }
    }
  }

  test_gate(5, PrimType::Or, vals);
}

TEST_F(SimNodeTest, NOR2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( x == 0 && y == 0 ) {
	val = 1;
      }
      else if ( x == 1 || y == 1 ) {
	val = 0;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::Nor, vals);
}

TEST_F(SimNodeTest, NOR3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 2;
	if ( x == 0 && y == 0 && z == 0 ) {
	  val = 1;
	}
	else if ( x == 1 || y == 1 || z == 1 ) {
	  val = 0;
	}
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::Nor, vals);
}

TEST_F(SimNodeTest, NOR4)
{
  int vals[3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  int val = 2;
	  if ( x == 0 && y == 0 && z == 0 && w == 0 ) {
	    val = 1;
	  }
	  else if ( x == 1 || y == 1 || z == 1 || w == 1 ) {
	    val = 0;
	  }
	  vals[((x * 3 + y) * 3 + z) * 3 + w] = val;
	}
      }
    }
  }

  test_gate(4, PrimType::Nor, vals);
}

TEST_F(SimNodeTest, NOR5)
{
  int vals[3 * 3 * 3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	for (int w = 0; w  < 3; ++ w) {
	  for (int u = 0; u < 3; ++ u) {
	    int val = 2;
	    if ( x == 0 && y == 0 && z == 0 && w == 0 && u == 0 ) {
	      val = 1;
	    }
	    else if ( x == 1 || y == 1 || z == 1 || w == 1 || u == 1 ) {
	      val = 0;
	    }
	    vals[(((x * 3 + y) * 3 + z) * 3 + w) * 3 + u] = val;
	  }
	}
      }
    }
  }

  test_gate(5, PrimType::Nor, vals);
}

TEST_F(SimNodeTest, XOR2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( (x == 0 && y == 1) ||
	   (x == 1 && y == 0) ) {
	val = 1;
      }
      else if ( (x == 0 && y == 0) ||
		(x == 1 && y == 1) ) {
	val = 0;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::Xor, vals);
}

TEST_F(SimNodeTest, XOR3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 0;
	if ( x == 1 ) {
	  val ^= 1;
	}
	else if ( x == 2 ) {
	  val = 2;
	  goto skip;
	}
	if ( y == 1 ) {
	  val ^= 1;
	}
	else if ( y == 2 ) {
	  val = 2;
	  goto skip;
	}
	if ( z == 1 ) {
	  val ^= 1;
	}
	else if ( z == 2 ) {
	  val = 2;
	}
      skip:
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::Xor, vals);
}

TEST_F(SimNodeTest, XNOR2)
{
  int vals[3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      int val = 2;
      if ( (x == 0 && y == 1) ||
	   (x == 1 && y == 0) ) {
	val = 0;
      }
      else if ( (x == 0 && y == 0) ||
		(x == 1 && y == 1) ) {
	val = 1;
      }
      vals[x * 3 + y] = val;
    }
  }

  test_gate(2, PrimType::Xnor, vals);
}

TEST_F(SimNodeTest, XNOR3)
{
  int vals[3 * 3 * 3];
  for (int x = 0; x < 3; ++ x) {
    for (int y = 0; y < 3; ++ y) {
      for (int z = 0; z < 3; ++ z) {
	int val = 1;
	if ( x == 1 ) {
	  val ^= 1;
	}
	else if ( x == 2 ) {
	  val = 2;
	  goto skip;
	}
	if ( y == 1 ) {
	  val ^= 1;
	}
	else if ( y == 2 ) {
	  val = 2;
	  goto skip;
	}
	if ( z == 1 ) {
	  val ^= 1;
	}
	else if ( z == 2 ) {
	  val = 2;
	}
      skip:
	vals[(x * 3 + y) * 3 + z] = val;
      }
    }
  }

  test_gate(3, PrimType::Xnor, vals);
}

END_NAMESPACE_DRUID_FSIM
