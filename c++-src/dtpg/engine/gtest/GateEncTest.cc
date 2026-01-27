
/// @file GateEncTest.cc
/// @brief GateEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "NetBuilder.h"
#include "GateEnc.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "types/FaultType.h"
#include "GateType.h"
#include "dtpg/VidMap.h"
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
    PrimType gate_type, ///< [in] ゲートの種類
    int vals[]          ///< [in] 真理値表を表す配列
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  TpgNetwork mNetwork;

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
  PrimType prim_type,
  int vals[]
)
{
  NetBuilder builder(FaultType::StuckAt);
  { // 入力数 input_num，ゲートタイプ prim_type のゲートを一つもつ
    // ネットワークを作る．
    std::vector<const NodeRep*> input_list(input_num);
    for ( SizeType i = 0; i < input_num; ++ i ) {
      input_list[i] = builder.make_input_node("");
    }
    auto gate_type = builder.make_gate_type(input_num, prim_type);
    auto gate = builder.make_gate(gate_type, input_list);
    auto onode = builder.make_output_node("", gate->output_node());
  }
  mNetwork = builder.wrap_up();
  mNetwork.print(std::cout);

  // 変数を割り当てる．
  VidMap varmap(mNetwork.node_num());
  for ( int i: Range(input_num) ) {
    auto var = mSolver.new_variable();
    auto inode = mNetwork.input(i);
    varmap.set_vid(inode, var);
  }
  auto ovar = mSolver.new_variable();
  auto onode = mNetwork.output(0);
  auto gnode = onode.fanin(0);
  varmap.set_vid(gnode, ovar);

  // 現在の CNF サイズ
  auto before_size = mSolver.cnf_size();

  // node の入出力の関係を表す CNF 式を生成する．
  GateEnc gate_enc(mSolver, varmap);
  gate_enc.make_cnf(gnode);

  // ノードの関係のCNFを追加した後のサイズ
  auto after_size = mSolver.cnf_size();

  // これが見積もり値と一致しているか調べる．
  EXPECT_EQ( after_size - before_size, GateEnc::calc_cnf_size(gnode) );

  std::vector<SatLiteral> assumptions(input_num + 1);
  int ni_exp = 1 << input_num;
  for ( int p: Range(ni_exp) ) {
    for ( int i: Range(input_num) ) {
      auto inode = mNetwork.input(i);
      auto lit = varmap(inode);
      if ( p & (1 << i) ) {
	assumptions[i] = lit;
      }
      else {
	assumptions[i] = ~lit;
      }
    }
    auto olit = varmap(gnode);

    // 正しい出力値を設定する．
    if ( vals[p] ) {
      assumptions[input_num] = olit;
    }
    else {
      assumptions[input_num] = ~olit;
    }
    auto res1 = mSolver.solve(assumptions);
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
  check(0, PrimType::C0, vals);
}

TEST_F(GateEncTest, const1)
{
  int vals[] = { 1 };
  check(0, PrimType::C1, vals);
}

TEST_F(GateEncTest, buff)
{
  int vals[] = { 0, 1 };
  check(1, PrimType::Buff, vals);
}

TEST_F(GateEncTest, not)
{
  int vals[] = { 1, 0 };
  check(1, PrimType::Not, vals);
}

TEST_F(GateEncTest, and2)
{
  int vals[] = { 0, 0, 0, 1 };
  check(2, PrimType::And, vals);
}

TEST_F(GateEncTest, and3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 1 };
  check(3, PrimType::And, vals);
}

TEST_F(GateEncTest, and4)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  check(4, PrimType::And, vals);
}

TEST_F(GateEncTest, and5)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  check(5, PrimType::And, vals);
}

TEST_F(GateEncTest, nand2)
{
  int vals[] = { 1, 1, 1, 0 };
  check(2, PrimType::Nand, vals);
}

TEST_F(GateEncTest, nand3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 0 };
  check(3, PrimType::Nand, vals);
}

TEST_F(GateEncTest, nand4)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
  check(4, PrimType::Nand, vals);
}

TEST_F(GateEncTest, nand5)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
  check(5, PrimType::Nand, vals);
}

TEST_F(GateEncTest, or2)
{
  int vals[] = { 0, 1, 1, 1 };
  check(2, PrimType::Or, vals);
}

TEST_F(GateEncTest, or3)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1 };
  check(3, PrimType::Or, vals);
}

TEST_F(GateEncTest, or4)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check(4, PrimType::Or, vals);
}

TEST_F(GateEncTest, or5)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check(5, PrimType::Or, vals);
}

TEST_F(GateEncTest, nor2)
{
  int vals[] = { 1, 0, 0, 0 };
  check(2, PrimType::Nor, vals);
}

TEST_F(GateEncTest, nor3)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0 };
  check(3, PrimType::Nor, vals);
}

TEST_F(GateEncTest, nor4)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check(4, PrimType::Nor, vals);
}

TEST_F(GateEncTest, nor5)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check(5, PrimType::Nor, vals);
}

TEST_F(GateEncTest, xor2)
{
  int vals[] = { 0, 1, 1, 0 };
  check(2, PrimType::Xor, vals);
}

TEST_F(GateEncTest, xnor2)
{
  int vals[] = { 1, 0, 0, 1 };
  check(2, PrimType::Xnor, vals);
}

END_NAMESPACE_DRUID
