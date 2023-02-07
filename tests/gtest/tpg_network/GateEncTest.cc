
/// @file GateEncTest.cc
/// @brief GateEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "GateEnc.h"
#include "GateType.h"
#include "TpgNetworkImpl.h"
#include "TpgGateInfo.h"
#include "TpgNode.h"
#include "VidMap.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

class GateEncTest :
  public ::testing::Test
{
public:

  /// @brief コンストラクタ
  GateEncTest();

  /// @brief デストラクタ
  ~GateEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
  ///
  /// 入力数は make_inputs() で設定した入力数を用いる．
  void
  check(
    SizeType input_num, ///< [in] 入力数
    GateType gate_type, ///< [in] ゲートの種類
    int vals[]          ///< [in] 真理値表を表す配列
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // TpgNetwork の中身
  TpgNetworkImpl mNetworkImpl;

};

GateEncTest::GateEncTest()
{
}

GateEncTest::~GateEncTest()
{
}

// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
void
GateEncTest::check(
  SizeType input_num,
  GateType gate_type,
  int vals[]
)
{
  mNetworkImpl.set_size(input_num, 0, 0, 1, 0);

  TpgGateInfoMgr gimgr;

  // 入力ノードを作る．
  vector<const TpgNode*> input_list(input_num);
  for ( int i: Range(input_num) ) {
    TpgNode* inode = mNetworkImpl.make_input_node(string{});
    input_list[i] = inode;
  }
  // ゲートを作る．
  vector<vector<const TpgNode*>> connection_list(mNetworkImpl.node_num());
  TpgNode* node = mNetworkImpl.make_logic_node(string(), gimgr.simple_type(gate_type),
					       input_list, connection_list);

  ASSERT_EQ( input_num + 1, mNetworkImpl.node_num() );

  // 変数を割り当てる．
  VidMap varmap(mNetworkImpl.node_num());
  for ( int i: Range(input_num) ) {
    SatLiteral var = mSolver.new_variable();
    varmap.set_vid(input_list[i], var);
  }
  {
    SatLiteral var = mSolver.new_variable();
    varmap.set_vid(node, var);
  }

  // node の入出力の関係を表す CNF 式を生成する．
  GateEnc gate_enc(mSolver, varmap);
  gate_enc.make_cnf(node);

  vector<SatLiteral> assumptions(input_num + 1);
  int ni_exp = 1 << input_num;
  for ( int p: Range(ni_exp) ) {
    for ( int i: Range(input_num) ) {
      SatLiteral lit = varmap(input_list[i]);
      if ( p & (1 << i) ) {
	assumptions[i] = lit;
      }
      else {
	assumptions[i] = ~lit;
      }
    }
    SatLiteral olit = varmap(node);

    // 正しい出力値を設定する．
    if ( vals[p] ) {
      assumptions[input_num] = olit;
    }
    else {
      assumptions[input_num] = ~olit;
    }
    SatBool3 res1 = mSolver.solve(assumptions);
    ASSERT_NE( SatBool3::X, res1 );
    EXPECT_EQ( SatBool3::True, res1 );

    // 誤った出力値を設定する．
    if ( vals[p] ) {
      assumptions[input_num] = ~olit;
    }
    else {
      assumptions[input_num] = olit;
    }
    SatBool3 res2 = mSolver.solve(assumptions);
    ASSERT_NE( SatBool3::X, res2 );
    EXPECT_EQ( SatBool3::False, res2 );
  }
}

TEST_F(GateEncTest, const0)
{
  int vals[] = { 0 };
  check(0, GateType::Const0, vals);
}

TEST_F(GateEncTest, const1)
{
  int vals[] = { 1 };
  check(0, GateType::Const1, vals);
}

TEST_F(GateEncTest, buff)
{
  int vals[] = { 0, 1 };
  check(1, GateType::Buff, vals);
}

TEST_F(GateEncTest, not)
{
  int vals[] = { 1, 0 };
  check(1, GateType::Not, vals);
}

TEST_F(GateEncTest, and2)
{
  int vals[] = { 0, 0, 0, 1 };
  check(2, GateType::And, vals);
}

TEST_F(GateEncTest, and3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 1 };
  check(3, GateType::And, vals);
}

TEST_F(GateEncTest, and4)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  check(4, GateType::And, vals);
}

TEST_F(GateEncTest, and5)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  check(5, GateType::And, vals);
}

TEST_F(GateEncTest, nand2)
{
  int vals[] = { 1, 1, 1, 0 };
  check(2, GateType::Nand, vals);
}

TEST_F(GateEncTest, nand3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 0 };
  check(3, GateType::Nand, vals);
}

TEST_F(GateEncTest, nand4)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
  check(4, GateType::Nand, vals);
}

TEST_F(GateEncTest, nand5)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
  check(5, GateType::Nand, vals);
}

TEST_F(GateEncTest, or2)
{
  int vals[] = { 0, 1, 1, 1 };
  check(2, GateType::Or, vals);
}

TEST_F(GateEncTest, or3)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1 };
  check(3, GateType::Or, vals);
}

TEST_F(GateEncTest, or4)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check(4, GateType::Or, vals);
}

TEST_F(GateEncTest, or5)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check(5, GateType::Or, vals);
}

TEST_F(GateEncTest, nor2)
{
  int vals[] = { 1, 0, 0, 0 };
  check(2, GateType::Nor, vals);
}

TEST_F(GateEncTest, nor3)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0 };
  check(3, GateType::Nor, vals);
}

TEST_F(GateEncTest, nor4)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check(4, GateType::Nor, vals);
}

TEST_F(GateEncTest, nor5)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check(5, GateType::Nor, vals);
}

TEST_F(GateEncTest, xor2)
{
  int vals[] = { 0, 1, 1, 0 };
  check(2, GateType::Xor, vals);
}

TEST_F(GateEncTest, xnor2)
{
  int vals[] = { 1, 0, 0, 1 };
  check(2, GateType::Xnor, vals);
}

END_NAMESPACE_DRUID
