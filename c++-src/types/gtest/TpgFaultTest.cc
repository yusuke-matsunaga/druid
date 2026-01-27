
/// @file TpgFaultTest.cc
/// @brief TpgFault のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "types/TpgFault.h"
#include "types/TpgNetwork.h"
#include "NetBuilder.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultTest:
  public ::testing::Test
{
public:

  // 外部入力を用意する．
  void
  make_inputs(
    SizeType input_num
  );

  // プリミティブ型のゲートを一つもつネットワークを作る．
  TpgNetwork
  make_primitive(
    PrimType prim_type,
    SizeType input_num
  );

  // 複合型のゲートを一つもつネットワークを作る．
  TpgNetwork
  make_cplx(
    const Expr& expr
  );

  // ビルダ
  NetBuilder mBuilder{FaultType::StuckAt};

  // 入力のリスト
  std::vector<const NodeRep*> mInputList;

};

// 外部入力を用意する．
void
TpgFaultTest::make_inputs(
  SizeType input_num
)
{
  mInputList.clear();
  mInputList.reserve(input_num);
  for ( SizeType i = 0; i < input_num; ++ i ) {
    std::ostringstream buf;
    buf << "input" << i;
    auto name = buf.str();
    auto node = mBuilder.make_input_node(name);
    mInputList.push_back(node);
  }
}

// プリミティブ型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgFaultTest::make_primitive(
  PrimType prim_type,
  SizeType input_num
)
{
  make_inputs(input_num);
  auto gate_type = mBuilder.make_gate_type(input_num, prim_type);
  auto gate = mBuilder.make_gate(gate_type, mInputList);
  mBuilder.make_output_node("x", gate->output_node());

  return mBuilder.wrap_up();
}

// 複合型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgFaultTest::make_cplx(
  const Expr& expr
)
{
  auto input_num = expr.input_size();
  make_inputs(input_num);
  auto gate_type = mBuilder.make_gate_type(input_num, expr);
  auto gate = mBuilder.make_gate(gate_type, mInputList);
  mBuilder.make_output_node("x", gate->output_node());

  return mBuilder.wrap_up();
}


TEST_F(TpgFaultTest, buf_sa)
{
  auto network = make_primitive(PrimType::Buff, 1);
  ASSERT_TRUE( network.is_valid() );

  ASSERT_EQ( 3, network.gate_num() );
  auto gate = network.gate(1);

  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f0 = gate.stem_fault(fval);
    ASSERT_TRUE( f0.is_valid() );
    EXPECT_EQ( gate, f0.gate() );
    EXPECT_EQ( FaultType::StuckAt, f0.fault_type() );
    EXPECT_EQ( fval, f0.fval() );
    EXPECT_TRUE( f0.is_stem() );
    EXPECT_FALSE( f0.is_branch() );
    EXPECT_THROW( f0.branch_pos(),
		  std::logic_error );
    EXPECT_THROW( f0.input_vals(),
		  std::logic_error );
  }

  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f0 = gate.branch_fault(0, fval);
    ASSERT_TRUE( f0.is_valid() );
    EXPECT_EQ( gate, f0.gate() );
    EXPECT_EQ( FaultType::StuckAt, f0.fault_type() );
    EXPECT_EQ( fval, f0.fval() );
    EXPECT_FALSE( f0.is_stem() );
    EXPECT_TRUE( f0.is_branch() );
    EXPECT_EQ( 0, f0.branch_pos() );
    EXPECT_THROW( f0.input_vals(),
		  std::logic_error );
  }
}

END_NAMESPACE_DRUID
