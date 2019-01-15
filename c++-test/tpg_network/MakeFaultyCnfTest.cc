
/// @file MakeFaultyCnfTest.cc
/// @brief MakeFaultyCnfTest の実装ファイル
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

class MakeFaultyCnfTest :
public ::testing::Test
{
public:

  /// @brief コンストラクタ
  MakeFaultyCnfTest() { }

  /// @brief テストを行う．
  /// @param[in] ni 入力数
  /// @param[in] gate_type ゲートタイプ
  /// @param[in] fpos 故障位置
  /// @param[in] fval 故障値 ( 0 / 1 )
  /// @param[in] vals 期待値の配列
  void
  do_test(ymuint ni,
	  GateType gate_type,
	  ymuint fpos,
	  int fval,
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
// @param[in] fpos 故障位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] vals 期待値のベクタ
void
MakeFaultyCnfTest::do_test(ymuint ni,
			   GateType gate_type,
			   ymuint fpos,
			   int fval,
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

  prim_node->make_faulty_cnf(mSolver, fpos, fval, lit_map);

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
  }
}


TEST_F(MakeFaultyCnfTest, BUFF_0_0)
{
  int vals[] = {
    0,
    0,
  };

  do_test(1, kGateBUFF, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, BUFF_0_1)
{
  int vals[] = {
    1,
    1,
  };

  do_test(1, kGateBUFF, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NOT_0_0)
{
  int vals[] = {
    1,
    1
  };

  do_test(1, kGateNOT, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOT_0_1)
{
  int vals[] = {
    0,
    0
  };

  do_test(1, kGateNOT, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND2_0_1)
{
  int vals[] = {
    0,
    0,
    1,
    1
  };

  do_test(2, kGateAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND2_1_1)
{
  int vals[] = {
    0,
    1,
    0,
    1
  };

  do_test(2, kGateAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND3_0_1)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1
  };

  do_test(3, kGateAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND3_1_1)
{
  int vals[] = {
    0,
    0,
    0,
    0,
    0,
    1,
    0,
    1
  };

  do_test(3, kGateAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND3_2_1)
{
  int vals[] = {
    0,
    0,
    0,
    1,
    0,
    0,
    0,
    1
  };

  do_test(3, kGateAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND4_0_1)
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
    1,
    1
  };

  do_test(4, kGateAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND4_1_1)
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
    1,
    0,
    1
  };

  do_test(4, kGateAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND4_2_1)
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
    1,
    0,
    0,
    0,
    1
  };

  do_test(4, kGateAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND4_3_1)
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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1
  };

  do_test(4, kGateAND, 3, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND5_0_1)
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
    1,
    1
  };

  do_test(5, kGateAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND5_1_1)
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
    1,
    0,
    1
  };

  do_test(5, kGateAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND5_2_1)
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
    1,
    0,
    0,
    0,
    1
  };

  do_test(5, kGateAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND5_3_1)
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
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1
  };

  do_test(5, kGateAND, 3, 1, vals);
}

TEST_F(MakeFaultyCnfTest, AND5_4_1)
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

  do_test(5, kGateAND, 4, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND2_0_1)
{
  int vals[] = {
    1,
    1,
    0,
    0
  };

  do_test(2, kGateNAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND2_1_1)
{
  int vals[] = {
    1,
    0,
    1,
    0
  };

  do_test(2, kGateNAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND3_0_1)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0
  };

  do_test(3, kGateNAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND3_1_1)
{
  int vals[] = {
    1,
    1,
    1,
    1,
    1,
    0,
    1,
    0
  };

  do_test(3, kGateNAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND3_2_1)
{
  int vals[] = {
    1,
    1,
    1,
    0,
    1,
    1,
    1,
    0
  };

  do_test(3, kGateNAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND4_0_1)
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
    0,
    0
  };

  do_test(4, kGateNAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND4_1_1)
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
    0,
    1,
    0
  };

  do_test(4, kGateNAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND4_2_1)
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
    0,
    1,
    1,
    1,
    0
  };

  do_test(4, kGateNAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND4_3_1)
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
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0
  };

  do_test(4, kGateNAND, 3, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND5_0_1)
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
    0,
    0
  };

  do_test(5, kGateNAND, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND5_1_1)
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
    0,
    1,
    0
  };

  do_test(5, kGateNAND, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND5_2_1)
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
    0,
    1,
    1,
    1,
    0
  };

  do_test(5, kGateNAND, 2, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND5_3_1)
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
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0
  };

  do_test(5, kGateNAND, 3, 1, vals);
}

TEST_F(MakeFaultyCnfTest, NAND5_4_1)
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

  do_test(5, kGateNAND, 4, 1, vals);
}

TEST_F(MakeFaultyCnfTest, OR2_0_0)
{
  int vals[] = {
    0,
    0,
    1,
    1
  };

  do_test(2, kGateOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR2_1_0)
{
  int vals[] = {
    0,
    1,
    0,
    1
  };

  do_test(2, kGateOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR3_0_0)
{
  int vals[] = {
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1
  };

  do_test(3, kGateOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR3_1_0)
{
  int vals[] = {
    0,
    1,
    0,
    1,
    1,
    1,
    1,
    1
  };

  do_test(3, kGateOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR3_2_0)
{
  int vals[] = {
    0,
    1,
    1,
    1,
    0,
    1,
    1,
    1
  };

  do_test(3, kGateOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR4_0_0)
{
  int vals[] = {
    0,
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
    1
  };

  do_test(4, kGateOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR4_1_0)
{
  int vals[] = {
    0,
    1,
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
    1
  };

  do_test(4, kGateOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR4_2_0)
{
  int vals[] = {
    0,
    1,
    1,
    1,
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
    1
  };

  do_test(4, kGateOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR4_3_0)
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
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1
  };

  do_test(4, kGateOR, 3, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR5_0_0)
{
  int vals[] = {
    0,
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
    1
  };

  do_test(5, kGateOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR5_1_0)
{
  int vals[] = {
    0,
    1,
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
    1
  };

  do_test(5, kGateOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR5_2_0)
{
  int vals[] = {
    0,
    1,
    1,
    1,
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
    1
  };

  do_test(5, kGateOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR5_3_0)
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
    1
  };

  do_test(5, kGateOR, 3, 0, vals);
}

TEST_F(MakeFaultyCnfTest, OR5_4_0)
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

  do_test(5, kGateOR, 4, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR2_0_0)
{
  int vals[] = {
    1,
    1,
    0,
    0
  };

  do_test(2, kGateNOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR2_1_0)
{
  int vals[] = {
    1,
    0,
    1,
    0
  };

  do_test(2, kGateNOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR3_0_0)
{
  int vals[] = {
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0
  };

  do_test(3, kGateNOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR3_1_0)
{
  int vals[] = {
    1,
    0,
    1,
    0,
    0,
    0,
    0,
    0
  };

  do_test(3, kGateNOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR3_2_0)
{
  int vals[] = {
    1,
    0,
    0,
    0,
    1,
    0,
    0,
    0
  };

  do_test(3, kGateNOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR4_0_0)
{
  int vals[] = {
    1,
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
    0
  };

  do_test(4, kGateNOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR4_1_0)
{
  int vals[] = {
    1,
    0,
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
    0
  };

  do_test(4, kGateNOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR4_2_0)
{
  int vals[] = {
    1,
    0,
    0,
    0,
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
    0
  };

  do_test(4, kGateNOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR4_3_0)
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
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };

  do_test(4, kGateNOR, 3, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR5_0_0)
{
  int vals[] = {
    1,
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
    0
  };

  do_test(5, kGateNOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR5_1_0)
{
  int vals[] = {
    1,
    0,
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
    0
  };

  do_test(5, kGateNOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR5_2_0)
{
  int vals[] = {
    1,
    0,
    0,
    0,
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
    0
  };

  do_test(5, kGateNOR, 2, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR5_3_0)
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
    0
  };

  do_test(5, kGateNOR, 3, 0, vals);
}

TEST_F(MakeFaultyCnfTest, NOR5_4_0)
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

  do_test(5, kGateNOR, 4, 0, vals);
}

TEST_F(MakeFaultyCnfTest, XOR2_0_0)
{
  int vals[] = {
    0,
    0,
    1,
    1
  };

  do_test(2, kGateXOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, XOR2_0_1)
{
  int vals[] = {
    1,
    1,
    0,
    0
  };

  do_test(2, kGateXOR, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, XOR2_1_0)
{
  int vals[] = {
    0,
    1,
    0,
    1
  };

  do_test(2, kGateXOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, XOR2_1_1)
{
  int vals[] = {
    1,
    0,
    1,
    0
  };

  do_test(2, kGateXOR, 1, 1, vals);
}

TEST_F(MakeFaultyCnfTest, XNOR2_0_0)
{
  int vals[] = {
    1,
    1,
    0,
    0
  };

  do_test(2, kGateXNOR, 0, 0, vals);
}

TEST_F(MakeFaultyCnfTest, XNOR2_0_1)
{
  int vals[] = {
    0,
    0,
    1,
    1
  };

  do_test(2, kGateXNOR, 0, 1, vals);
}

TEST_F(MakeFaultyCnfTest, XNOR2_1_0)
{
  int vals[] = {
    1,
    0,
    1,
    0
  };

  do_test(2, kGateXNOR, 1, 0, vals);
}

TEST_F(MakeFaultyCnfTest, XNOR2_1_1)
{
  int vals[] = {
    0,
    1,
    0,
    1
  };

  do_test(2, kGateXNOR, 1, 1, vals);
}

END_NAMESPACE_DRUID
