
/// @file TpgNetworkTest3.cc
/// @brief TpgNetworkTest3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "NetBuilder.h"
#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkTest3 :
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

  // 故障の内容を文字列にする．
  std::string
  print_faults(
    const TpgFaultList& fault_list
  );

  // BnModel
  BnModel mModel;

  // 入力のリスト
  std::vector<BnNode> mInputList;

};


// 外部入力を用意する．
void
TpgNetworkTest3::make_inputs(
  SizeType input_num
)
{
  mInputList.clear();
  mInputList.reserve(input_num);
  for ( SizeType i = 0; i < input_num; ++ i ) {
    std::ostringstream buf;
    buf << "input" << i;
    auto name = buf.str();
    auto node = mModel.new_input(name);
    mInputList.push_back(node);
  }
}

// プリミティブ型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgNetworkTest3::make_primitive(
  PrimType prim_type,
  SizeType input_num
)
{
  make_inputs(input_num);
  auto node = mModel.new_primitive(prim_type, mInputList);
  mModel.new_output(node, "output");
  return TpgNetwork::from_bn(mModel, FaultType::StuckAt);
}

// 複合型のゲートを一つもつネットワークを作る．
TpgNetwork
TpgNetworkTest3::make_cplx(
  const Expr& expr
)
{
  auto input_num = expr.input_size();
  make_inputs(input_num);
  auto node = mModel.new_expr(expr, mInputList);
  mModel.new_output(node, "output");
  return TpgNetwork::from_bn(mModel, FaultType::StuckAt);
}

// 故障の内容を文字列にする．
std::string
TpgNetworkTest3::print_faults(
  const TpgFaultList& fault_list
)
{
  auto node_fault_list_array = fault_list.node_split();
  auto ffr_fault_list_array = fault_list.ffr_split();
  auto mffc_fault_list_array = fault_list.mffc_split();

  std::ostringstream s;

  s << "fault_list" << std::endl;
  for ( auto fault: fault_list ) {
    s << " " << fault.str();
  }
  s << std::endl;

  s << "node_fault_list_array" << std::endl;
  auto nn = node_fault_list_array.size();
  for ( SizeType i = 0; i < nn; ++ i ) {
    auto& fault_list = node_fault_list_array[i];
    s << " Node#" << i;
    for ( auto fault: fault_list ) {
      s << " " << fault.str();
    }
    s << std::endl;
  }

  s << "ffr_fault_list_array" << std::endl;
  auto nf = ffr_fault_list_array.size();
  for ( SizeType i = 0; i < nf; ++ i ) {
    auto& fault_list = ffr_fault_list_array[i];
    s << " FFR#" << i;
    for ( auto fault: fault_list ) {
      s << " " << fault.str();
    }
    s << std::endl;
  }

  s << "mffc_fault_list_array" << std::endl;
  auto nm = mffc_fault_list_array.size();
  for ( SizeType i = 0; i < nm; ++ i ) {
    auto p = mffc_fault_list_array[i];
    auto ffr = p.first;
    auto& fault_list = p.second;
    s << " MFFC#" << i;
    if ( ffr.is_valid() ) {
      s << "(FFR#" << ffr.id() << ")";
    }
    for ( auto fault: fault_list ) {
      s << " " << fault.str();
    }
    s << std::endl;
  }

  return s.str();
}

TEST_F(TpgNetworkTest3, xor2)
{
  // 2入力XORゲート1つからなるネットワークを作る．
  auto tpg_network = make_primitive(PrimType::Xor, 2);
  ASSERT_TRUE( tpg_network.is_valid() );

  auto fault_list = tpg_network.rep_fault_list();

  auto s = print_faults(fault_list);

  std::string ref_str =
    "fault_list\n"
    " G#2:I0:SA0 G#2:I0:SA1 G#2:I1:SA0 G#2:I1:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "node_fault_list_array\n"
    " Node#0\n"
    " Node#1\n"
    " Node#2 G#2:I0:SA0 G#2:I0:SA1 G#2:I1:SA0 G#2:I1:SA1\n"
    " Node#3 O#0:I0:SA0 O#0:I0:SA1\n"
    "ffr_fault_list_array\n"
    " FFR#0 G#2:I0:SA0 G#2:I0:SA1 G#2:I1:SA0 G#2:I1:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "mffc_fault_list_array\n"
    " MFFC#0(FFR#0) G#2:I0:SA0 G#2:I0:SA1 G#2:I1:SA0 G#2:I1:SA1 O#0:I0:SA0 O#0:I0:SA1\n";
  EXPECT_EQ( ref_str, s );
}

TEST_F(TpgNetworkTest3, aoi22)
{
  // AOI221つからなるネットワークを作る．
  auto a1_lit = Expr::positive_literal(0);
  auto a2_lit = Expr::positive_literal(1);
  auto b1_lit = Expr::positive_literal(2);
  auto b2_lit = Expr::positive_literal(3);
  auto aoi22_expr = ~((a1_lit & a2_lit) | (b1_lit & b2_lit));
  auto tpg_network = make_cplx(aoi22_expr);
  ASSERT_TRUE( tpg_network.is_valid() );

  auto fault_list = tpg_network.rep_fault_list();
  auto s = print_faults(fault_list);

  std::string ref_str =
    "fault_list\n"
    " G#4:I0:SA0 G#4:I0:SA1 G#4:I1:SA0 G#4:I1:SA1 G#4:I2:SA0 G#4:I2:SA1 G#4:I3:SA0 G#4:I3:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "node_fault_list_array\n"
    " Node#0\n"
    " Node#1\n"
    " Node#2\n"
    " Node#3\n"
    " Node#4 G#4:I0:SA0 G#4:I0:SA1\n"
    " Node#5 G#4:I1:SA0 G#4:I1:SA1\n"
    " Node#6 G#4:I2:SA0 G#4:I2:SA1\n"
    " Node#7 G#4:I3:SA0 G#4:I3:SA1\n"
    " Node#8\n"
    " Node#9\n"
    " Node#10\n"
    " Node#11 O#0:I0:SA0 O#0:I0:SA1\n"
    "ffr_fault_list_array\n"
    " FFR#0 G#4:I0:SA0 G#4:I0:SA1 G#4:I1:SA0 G#4:I1:SA1 G#4:I2:SA0 G#4:I2:SA1 G#4:I3:SA0 G#4:I3:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "mffc_fault_list_array\n"
    " MFFC#0(FFR#0) G#4:I0:SA0 G#4:I0:SA1 G#4:I1:SA0 G#4:I1:SA1 G#4:I2:SA0 G#4:I2:SA1 G#4:I3:SA0 G#4:I3:SA1 O#0:I0:SA0 O#0:I0:SA1\n";

  EXPECT_EQ( ref_str, s );
}

TEST_F(TpgNetworkTest3, nand_xor2)
{
  // NAND2x4 でXORゲートを構成したネットワークを作る．
  make_inputs(2);
  auto nand1 = mModel.new_primitive(PrimType::Nand, mInputList);
  auto nand2 = mModel.new_primitive(PrimType::Nand, {mInputList[0], nand1});
  auto nand3 = mModel.new_primitive(PrimType::Nand, {mInputList[1], nand1});
  auto nand4 = mModel.new_primitive(PrimType::Nand, {nand2, nand3});
  mModel.new_output(nand4, "output");
  auto tpg_network = TpgNetwork::from_bn(mModel, FaultType::StuckAt);
  ASSERT_TRUE( tpg_network.is_valid() );

  auto fault_list = tpg_network.rep_fault_list();
  auto s = print_faults(fault_list);

  std::string ref_str =
    "fault_list\n"
    " I#0:O:SA0 I#0:O:SA1 I#1:O:SA0 I#1:O:SA1 G#2:O:SA0 G#2:O:SA1 G#2:I0:SA1 G#2:I1:SA1 G#3:I0:SA1 G#3:I1:SA1 G#4:I0:SA1 G#4:I1:SA1 G#5:I0:SA1 G#5:I1:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "node_fault_list_array\n"
    " Node#0 I#0:O:SA0 I#0:O:SA1\n"
    " Node#1 I#1:O:SA0 I#1:O:SA1\n"
    " Node#2 G#2:O:SA0 G#2:O:SA1 G#2:I0:SA1 G#2:I1:SA1\n"
    " Node#3 G#3:I0:SA1 G#3:I1:SA1\n"
    " Node#4 G#4:I0:SA1 G#4:I1:SA1\n"
    " Node#5 G#5:I0:SA1 G#5:I1:SA1\n"
    " Node#6 O#0:I0:SA0 O#0:I0:SA1\n"
    "ffr_fault_list_array\n"
    " FFR#0 I#0:O:SA0 I#0:O:SA1\n"
    " FFR#1 I#1:O:SA0 I#1:O:SA1\n"
    " FFR#2 G#2:O:SA0 G#2:O:SA1 G#2:I0:SA1 G#2:I1:SA1\n"
    " FFR#3 G#3:I0:SA1 G#3:I1:SA1 G#4:I0:SA1 G#4:I1:SA1 G#5:I0:SA1 G#5:I1:SA1 O#0:I0:SA0 O#0:I0:SA1\n"
    "mffc_fault_list_array\n"
    " MFFC#0 G#3:I0:SA1 G#3:I1:SA1 G#4:I0:SA1 G#4:I1:SA1 G#5:I0:SA1 G#5:I1:SA1 O#0:I0:SA0 O#0:I0:SA1 G#2:O:SA0 G#2:O:SA1 G#2:I0:SA1 G#2:I1:SA1 I#0:O:SA0 I#0:O:SA1 I#1:O:SA0 I#1:O:SA1\n";

  EXPECT_EQ( ref_str, s );
}

END_NAMESPACE_DRUID
