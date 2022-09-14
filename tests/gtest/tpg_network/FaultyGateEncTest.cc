
/// @file FaultyGateEncTest.cc
/// @brief FaultyGateEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "FaultyGateEnc.h"
#include "GateType.h"
#include "TpgNetworkImpl.h"
#include "TpgGateInfo.h"
#include "TpgNode.h"
#include "TpgFaultBase.h"
#include "VidMap.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

class FaultyGateEncTest :
  public ::testing::Test
{
public:

  /// @brief コンストラクタ
  FaultyGateEncTest();

  /// @brief デストラクタ
  ~FaultyGateEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
  ///
  /// 入力数は make_inputs() で設定した入力数を用いる．
  void
  check_ofault(
    SizeType input_num, ///< [in] 入力数
    GateType gate_type, ///< [in] ゲートの種類
    Fval2 val,          ///< [in] 出力の故障値
    int vals[]          ///< [in] 真理値表を表す配列
  );

  /// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
  ///
  /// 入力数は make_inputs() で設定した入力数を用いる．
  void
  check_ifault(
    SizeType input_num, ///< [in] 入力数
    GateType gate_type, ///< [in] ゲートの種類
    Fval2 val,          ///< [in] 入力の故障値
    SizeType fpos,      ///< [in] 入力の故障位置
    int vals[]);        ///< [in] 真理値表を表す配列


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // TpgNetwork の中身
  TpgNetworkImpl mNetworkImpl;

};

FaultyGateEncTest::FaultyGateEncTest()
{
}

FaultyGateEncTest::~FaultyGateEncTest()
{
}

// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
void
FaultyGateEncTest::check_ofault(
  SizeType input_num,
  GateType gate_type,
  Fval2 val,
  int vals[]
)
{
  mNetworkImpl.set_size(input_num, 0, 0, input_num + 1);

  TpgGateInfoMgr gimgr;

  // 入力ノードを作る．
  vector<const TpgNode*> input_list(input_num);
  for ( int i: Range(input_num) ) {
    TpgNode* inode = mNetworkImpl.make_input_node(i, string(), 1);
    input_list[i] = inode;
  }
  // ゲートを作る．
  vector<pair<SizeType, SizeType>> connection_list;
  TpgNode* node = mNetworkImpl.make_logic_node(string(), gimgr.simple_type(gate_type),
					       input_list, 0, connection_list);

  ASSERT_EQ( input_num + 1, mNetworkImpl.node_num() );

  // 変数を割り当てる．
  VidMap varmap(mNetworkImpl.node_num());
  for ( int i: Range(input_num) ) {
    auto var = mSolver.new_variable();
    varmap.set_vid(input_list[i], var);
  }
  {
    auto var = mSolver.new_variable();
    varmap.set_vid(node, var);
  }

  const TpgFault* fault = mNetworkImpl._node_output_fault(node->id(), val);
  // node の入出力の関係を表す CNF 式を生成する．
  FaultyGateEnc gate_enc(mSolver, varmap, fault);
  gate_enc.make_cnf();

  vector<SatLiteral> assumptions(input_num + 1);
  int ni_exp = 1 << input_num;
  for ( int p: Range(ni_exp) ) {
    for ( int i: Range(input_num) ) {
      auto lit = varmap(input_list[i]);
      if ( p & (1 << i) ) {
	assumptions[i] = lit;
      }
      else {
	assumptions[i] = ~lit;
      }
    }
    auto olit = varmap(node);

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

// @brief 設定された CNF が vals[] で示された真理値表と等しいか調べる．
void
FaultyGateEncTest::check_ifault(
  SizeType input_num,
  GateType gate_type,
  Fval2 val,
  SizeType fpos,
  int vals[]
)
{
  mNetworkImpl.set_size(input_num, 0, 0, input_num + 1);

  TpgGateInfoMgr gimgr;

  // 入力ノードを作る．
  vector<const TpgNode*> input_list(input_num);
  for ( int i: Range(input_num) ) {
    TpgNode* inode = mNetworkImpl.make_input_node(i, string(), 1);
    input_list[i] = inode;
  }
  // ゲートを作る．
  vector<pair<SizeType, SizeType>> connection_list;
  TpgNode* node = mNetworkImpl.make_logic_node(string(), gimgr.simple_type(gate_type),
					       input_list, 0, connection_list);

  ASSERT_EQ( input_num + 1, mNetworkImpl.node_num() );

  // 変数を割り当てる．
  VidMap varmap(mNetworkImpl.node_num());
  for ( int i: Range(input_num) ) {
    auto var = mSolver.new_variable();
    varmap.set_vid(input_list[i], var);
  }
  {
    auto var = mSolver.new_variable();
    varmap.set_vid(node, var);
  }

  const TpgFault* fault = mNetworkImpl._node_input_fault(node->id(), val, fpos);
  // node の入出力の関係を表す CNF 式を生成する．
  FaultyGateEnc gate_enc(mSolver, varmap, fault);
  gate_enc.make_cnf();

  vector<SatLiteral> assumptions(input_num + 1);
  int ni_exp = 1 << input_num;
  for ( int p: Range(ni_exp) ) {
    for ( int i: Range(input_num) ) {
      auto lit = varmap(input_list[i]);
      if ( p & (1 << i) ) {
	assumptions[i] = lit;
      }
      else {
	assumptions[i] = ~lit;
      }
    }
    auto olit = varmap(node);

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

TEST_F(FaultyGateEncTest, const0_0_o)
{
  int vals[] = { 0 };
  check_ofault(0, GateType::Const0, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, const0_1_o)
{
  int vals[] = { 1 };
  check_ofault(0, GateType::Const0, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, const1_0_o)
{
  int vals[] = { 0 };
  check_ofault(0, GateType::Const1, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, const1_1_o)
{
  int vals[] = { 1 };
  check_ofault(0, GateType::Const1, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, buff_0_o)
{
  int vals[] = { 0, 0 };
  check_ofault(1, GateType::Buff, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, buff_1_o)
{
  int vals[] = { 1, 1 };
  check_ofault(1, GateType::Buff, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, buff_0_i0)
{
  int vals[] = { 0, 0 };
  check_ifault(1, GateType::Buff, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, buff_1_i0)
{
  int vals[] = { 1, 1 };
  check_ifault(1, GateType::Buff, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, not_0_o)
{
  int vals[] = { 0, 0 };
  check_ofault(1, GateType::Not, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, not_1_o)
{
  int vals[] = { 1, 1 };
  check_ofault(1, GateType::Not, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, not_0_i0)
{
  int vals[] = { 1, 1 };
  check_ifault(1, GateType::Not, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, not_1_i0)
{
  int vals[] = { 0, 0 };
  check_ifault(1, GateType::Not, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, and2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::And, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, and2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::And, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, and2_0_i0)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ifault(2, GateType::And, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, and2_1_i0)
{
  int vals[] = { 0, 0, 1, 1 };
  check_ifault(2, GateType::And, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, and2_0_i1)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ifault(2, GateType::And, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, and2_1_i1)
{
  int vals[] = { 0, 1, 0, 1 };
  check_ifault(2, GateType::And, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, and3_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(3, GateType::And, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, and3_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(3, GateType::And, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, and3_0_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::And, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, and3_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 1, 1 };
  check_ifault(3, GateType::And, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, and3_0_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::And, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, and3_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 1, 0, 1 };
  check_ifault(3, GateType::And, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, and3_0_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::And, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, and3_1_i2)
{
  int vals[] = { 0, 0, 0, 1, 0, 0, 0, 1 };
  check_ifault(3, GateType::And, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, and4_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(4, GateType::And, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, and4_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(4, GateType::And, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, and4_0_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::And, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, and4_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };
  check_ifault(4, GateType::And, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, and4_0_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::And, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, and4_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };
  check_ifault(4, GateType::And, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, and4_0_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::And, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, and4_1_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 };
  check_ifault(4, GateType::And, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, and4_0_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::And, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, and4_1_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
  check_ifault(4, GateType::And, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, and5_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(5, GateType::And, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, and5_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(5, GateType::And, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, and5_0_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::And, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, and5_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 };
  check_ifault(5, GateType::And, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, and5_0_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::And, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, and5_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };
  check_ifault(5, GateType::And, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, and5_0_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::And, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, and5_1_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 };
  check_ifault(5, GateType::And, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, and5_0_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::And, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, and5_1_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };
  check_ifault(5, GateType::And, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, and5_0_i4)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::And, Fval2::zero, 4, vals);
}

TEST_F(FaultyGateEncTest, and5_1_i4)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  check_ifault(5, GateType::And, Fval2::one, 4, vals);
}

TEST_F(FaultyGateEncTest, nand2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::Nand, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nand2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::Nand, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nand2_0_i0)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ifault(2, GateType::Nand, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nand2_1_i0)
{
  int vals[] = { 1, 1, 0, 0 };
  check_ifault(2, GateType::Nand, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nand2_0_i1)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ifault(2, GateType::Nand, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nand2_1_i1)
{
  int vals[] = { 1, 0, 1, 0 };
  check_ifault(2, GateType::Nand, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nand3_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(3, GateType::Nand, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nand3_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(3, GateType::Nand, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nand3_0_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Nand, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nand3_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 0, 0 };
  check_ifault(3, GateType::Nand, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nand3_0_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Nand, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nand3_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 0, 1, 0 };
  check_ifault(3, GateType::Nand, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nand3_0_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Nand, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nand3_1_i2)
{
  int vals[] = { 1, 1, 1, 0, 1, 1, 1, 0 };
  check_ifault(3, GateType::Nand, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nand4_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(4, GateType::Nand, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nand4_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(4, GateType::Nand, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nand4_0_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Nand, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nand4_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
  check_ifault(4, GateType::Nand, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nand4_0_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Nand, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nand4_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
  check_ifault(4, GateType::Nand, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nand4_0_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Nand, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nand4_1_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0 };
  check_ifault(4, GateType::Nand, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nand4_0_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Nand, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, nand4_1_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0 };
  check_ifault(4, GateType::Nand, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(5, GateType::Nand, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(5, GateType::Nand, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Nand, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 };
  check_ifault(5, GateType::Nand, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Nand, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 };
  check_ifault(5, GateType::Nand, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Nand, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0 };
  check_ifault(5, GateType::Nand, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Nand, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0 };
  check_ifault(5, GateType::Nand, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, nand5_0_i4)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Nand, Fval2::zero, 4, vals);
}

TEST_F(FaultyGateEncTest, nand5_1_i4)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
  check_ifault(5, GateType::Nand, Fval2::one, 4, vals);
}

TEST_F(FaultyGateEncTest, or2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::Or, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, or2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::Or, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, or2_0_i0)
{
  int vals[] = { 0, 0, 1, 1 };
  check_ifault(2, GateType::Or, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, or2_1_i0)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ifault(2, GateType::Or, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, or2_0_i1)
{
  int vals[] = { 0, 1, 0, 1 };
  check_ifault(2, GateType::Or, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, or2_1_i1)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ifault(2, GateType::Or, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, or3_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(3, GateType::Or, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, or3_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(3, GateType::Or, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, or3_0_i0)
{
  int vals[] = { 0, 0, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, or3_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, or3_0_i1)
{
  int vals[] = { 0, 1, 0, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, or3_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, or3_0_i2)
{
  int vals[] = { 0, 1, 1, 1, 0, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, or3_1_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(3, GateType::Or, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, or4_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(4, GateType::Or, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, or4_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(4, GateType::Or, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, or4_0_i0)
{
  int vals[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, or4_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, or4_0_i1)
{
  int vals[] = { 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, or4_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, or4_0_i2)
{
  int vals[] = { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, or4_1_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, or4_0_i3)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, or4_1_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(4, GateType::Or, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, or5_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(5, GateType::Or, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, or5_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(5, GateType::Or, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, or5_0_i0)
{
  int vals[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, or5_1_i0)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, or5_0_i1)
{
  int vals[] = { 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, or5_1_i1)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, or5_0_i2)
{
  int vals[] = { 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, or5_1_i2)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, or5_0_i3)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, or5_1_i3)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, or5_0_i4)
{
  int vals[] = { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::zero, 4, vals);
}

TEST_F(FaultyGateEncTest, or5_1_i4)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ifault(5, GateType::Or, Fval2::one, 4, vals);
}

TEST_F(FaultyGateEncTest, nor2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::Nor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nor2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::Nor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nor2_0_i0)
{
  int vals[] = { 1, 1, 0, 0 };
  check_ifault(2, GateType::Nor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nor2_1_i0)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ifault(2, GateType::Nor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nor2_0_i1)
{
  int vals[] = { 1, 0, 1, 0 };
  check_ifault(2, GateType::Nor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nor2_1_i1)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ifault(2, GateType::Nor, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nor3_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(3, GateType::Nor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nor3_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(3, GateType::Nor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nor3_0_i0)
{
  int vals[] = { 1, 1, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nor3_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nor3_0_i1)
{
  int vals[] = { 1, 0, 1, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nor3_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nor3_0_i2)
{
  int vals[] = { 1, 0, 0, 0, 1, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nor3_1_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(3, GateType::Nor, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nor4_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(4, GateType::Nor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nor4_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(4, GateType::Nor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nor4_0_i0)
{
  int vals[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nor4_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nor4_0_i1)
{
  int vals[] = { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nor4_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nor4_0_i2)
{
  int vals[] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nor4_1_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nor4_0_i3)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, nor4_1_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(4, GateType::Nor, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_o)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ofault(5, GateType::Nor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_o)
{
  int vals[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  check_ofault(5, GateType::Nor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_i0)
{
  int vals[] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_i0)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_i1)
{
  int vals[] = { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_i1)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_i2)
{
  int vals[] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::zero, 2, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_i2)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::one, 2, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_i3)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::zero, 3, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_i3)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::one, 3, vals);
}

TEST_F(FaultyGateEncTest, nor5_0_i4)
{
  int vals[] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::zero, 4, vals);
}

TEST_F(FaultyGateEncTest, nor5_1_i4)
{
  int vals[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  check_ifault(5, GateType::Nor, Fval2::one, 4, vals);
}

TEST_F(FaultyGateEncTest, xor2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::Xor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, xor2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::Xor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, xor2_0_i0)
{
  int vals[] = { 0, 0, 1, 1 };
  check_ifault(2, GateType::Xor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, xor2_1_i0)
{
  int vals[] = { 1, 1, 0, 0 };
  check_ifault(2, GateType::Xor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, xor2_0_i1)
{
  int vals[] = { 0, 1, 0, 1 };
  check_ifault(2, GateType::Xor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, xor2_1_i1)
{
  int vals[] = { 1, 0, 1, 0 };
  check_ifault(2, GateType::Xor, Fval2::one, 1, vals);
}

TEST_F(FaultyGateEncTest, xnor2_0_o)
{
  int vals[] = { 0, 0, 0, 0 };
  check_ofault(2, GateType::Xnor, Fval2::zero, vals);
}

TEST_F(FaultyGateEncTest, xnor2_1_o)
{
  int vals[] = { 1, 1, 1, 1 };
  check_ofault(2, GateType::Xnor, Fval2::one, vals);
}

TEST_F(FaultyGateEncTest, xnor2_0_i0)
{
  int vals[] = { 1, 1, 0, 0 };
  check_ifault(2, GateType::Xnor, Fval2::zero, 0, vals);
}

TEST_F(FaultyGateEncTest, xnor2_1_i0)
{
  int vals[] = { 0, 0, 1, 1 };
  check_ifault(2, GateType::Xnor, Fval2::one, 0, vals);
}

TEST_F(FaultyGateEncTest, xnor2_0_i1)
{
  int vals[] = { 1, 0, 1, 0 };
  check_ifault(2, GateType::Xnor, Fval2::zero, 1, vals);
}

TEST_F(FaultyGateEncTest, xnor2_1_i1)
{
  int vals[] = { 0, 1, 0, 1 };
  check_ifault(2, GateType::Xnor, Fval2::one, 1, vals);
}

END_NAMESPACE_DRUID
