
/// @file MakeCnfTest.cc
/// @brief MakeCnfTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "druid.h"
#include "TpgNode.h"
#include "GateLitMap_vect.h"
#include "ym/SimpleAlloc.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

class MakeCnfTest :
public ::testing::Test
{
public:

  /// @brief コンストラクタ
  MakeCnfTest() : mSolver(string(), string()) { }

  /// @brief テストを行う．
  /// @param[in] ni 入力数
  /// @param[in] gate_type ゲートタイプ
  /// @param[in] vals 期待値の配列
  void
  do_test(ymuint ni,
	  GateType gate_type,
	  int* vals);


protected:

  // メモリアロケータ
  SimpleAlloc mAlloc;

  // SAT ソルバ
  SatSolver mSolver;

  // 入力ノードのベクタ
  vector<TpgNode*> mInputs;

  // 入力変数のベクタ
  vector<SatVarId> mIvars;

  // 出力変数
  SatVarId mOvar;

};

// @brief テストを行う．
// @param[in] ni 入力数
// @param[in] gate_type ゲートタイプ
// @param[in] vals 期待値のベクタ
void
MakeCnfTest::do_test(ymuint ni,
		     GateType gate_type,
		     int* vals)
{
  mInputs.clear();
  mInputs.resize(ni);
  mIvars.clear();
  mIvars.resize(ni);
  for (ymuint i = 0; i < ni; ++ i) {
    TpgNode* node = TpgNode::make_input(i, string(), i, 1, mAlloc);
    mInputs[i] = node;
    SatVarId var = mSolver.new_variable();
    mIvars[i] = var;
  }
  mOvar = mSolver.new_variable();

  TpgNode* prim_node = TpgNode::make_logic(ni, string(), gate_type, mInputs, 1, mAlloc);

  GateLitMap_vect lit_map(mIvars, mOvar);

  prim_node->make_cnf(mSolver, lit_map);

  ymuint np = (1 << ni);

  for (ymuint p = 0; p < np; ++ p) {
    vector<SatLiteral> assumptions;
    for (ymuint i = 0; i < ni; ++ i) {
      if ( p & (1 << i) ) {
	assumptions.push_back(SatLiteral(mIvars[i], false));
      }
      else {
	assumptions.push_back(SatLiteral(mIvars[i], true));
      }
    }
    if ( vals[p] ) {
      assumptions.push_back(SatLiteral(mOvar, false));
    }
    else {
      assumptions.push_back(SatLiteral(mOvar, true));
    }
    vector<SatBool3> model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( stat, kB3True );

    if ( vals[p] ) {
      assumptions[ni] = SatLiteral(mOvar, true);
    }
    else {
      assumptions[ni] = SatLiteral(mOvar, false);
    }
    SatBool3 stat2 = mSolver.solve(assumptions, model);
    EXPECT_EQ( stat2, kB3False );
  }
}


TEST_F(MakeCnfTest, const0)
{
  int vals[] = {
    0
  };
  do_test(0, kGateCONST0, vals);
}

TEST_F(MakeCnfTest, const1)
{
  int vals[] = {
    1
  };
  do_test(0, kGateCONST1, vals);
}

TEST_F(MakeCnfTest, BUFF)
{
  int vals[] = {
    0,
    1,
  };

  do_test(1, kGateBUFF, vals);
}

TEST_F(MakeCnfTest, NOT)
{
  int vals[] = {
    1,
    0
  };

  do_test(1, kGateNOT, vals);
}

TEST_F(MakeCnfTest, AND2)
{
  int vals[] = {
    0,
    0,
    0,
    1
  };

  do_test(2, kGateAND, vals);
}

TEST_F(MakeCnfTest, AND3)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1
  };

  do_test(3, kGateAND, vals);
}

TEST_F(MakeCnfTest, AND4)
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
    1
  };

  do_test(4, kGateAND, vals);
}

TEST_F(MakeCnfTest, AND5)
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
    1
  };

  do_test(5, kGateAND, vals);
}

TEST_F(MakeCnfTest, NAND2)
{
  int vals[] = {
    1,
    1,
    1,
    0
  };

  do_test(2, kGateNAND, vals);
}

TEST_F(MakeCnfTest, NAND3)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0
  };

  do_test(3, kGateNAND, vals);
}

TEST_F(MakeCnfTest, NAND4)
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
    0
  };

  do_test(4, kGateNAND, vals);
}

TEST_F(MakeCnfTest, NAND5)
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
    0
  };

  do_test(5, kGateNAND, vals);
}

TEST_F(MakeCnfTest, OR2)
{
  int vals[] = {
    0,
    1,
    1,
    1
  };

  do_test(2, kGateOR, vals);
}

TEST_F(MakeCnfTest, OR3)
{
  int vals[] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1
  };

  do_test(3, kGateOR, vals);
}

TEST_F(MakeCnfTest, OR4)
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
    1
  };

  do_test(4, kGateOR, vals);
}

TEST_F(MakeCnfTest, OR5)
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
    1
  };

  do_test(5, kGateOR, vals);
}

TEST_F(MakeCnfTest, NOR2)
{
  int vals[] = {
    1,
    0,
    0,
    0
  };

  do_test(2, kGateNOR, vals);
}

TEST_F(MakeCnfTest, NOR3)
{
  int vals[] = {
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };

  do_test(3, kGateNOR, vals);
}

TEST_F(MakeCnfTest, NOR4)
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
    0
  };

  do_test(4, kGateNOR, vals);
}

TEST_F(MakeCnfTest, NOR5)
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
    0
  };

  do_test(5, kGateNOR, vals);
}

TEST_F(MakeCnfTest, XOR2)
{
  int vals[] = {
    0,
    1,
    1,
    0
  };

  do_test(2, kGateXOR, vals);
}

TEST_F(MakeCnfTest, XNOR2)
{
  int vals[] = {
    1,
    0,
    0,
    1
  };

  do_test(2, kGateXNOR, vals);
}

END_NAMESPACE_DRUID
