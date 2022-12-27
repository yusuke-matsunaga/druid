
/// @file GateEnc.cc
/// @brief GateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "GateEnc.h"

#include "TpgNode.h"
#include "GateType.h"
#include "VidMap.h"

#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
GateEnc::GateEnc(
  SatSolver& solver,
  const VidMap& varmap
) : mSolver{solver},
    mVarMap{varmap}
{
}

// @brief デストラクタ
GateEnc::~GateEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(
  const TpgNode* node
)
{
  make_cnf(node, mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(
  const TpgNode* node,
  SatLiteral olit
)
{
  SizeType ni = node->fanin_num();
  const auto& fanin_array = node->fanin_list();
  switch ( node->gate_type() ) {
  case GateType::Const0:
    mSolver.add_clause(~olit);
    break;

  case GateType::Const1:
    mSolver.add_clause( olit);
    break;

  case GateType::Input:
    // なにもしない．
    break;

  case GateType::Buff:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_buffgate(ilit, olit);
    }
    break;

  case GateType::Not:
    {
      auto ilit = lit(fanin_array[0]);
      mSolver.add_notgate(ilit, olit);
    }
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_andgate(olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_andgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_andgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_andgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_nandgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_nandgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_nandgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_nandgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_orgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_orgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_orgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_orgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	mSolver.add_norgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	mSolver.add_norgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	auto ilit0 = lit(fanin_array[0]);
	auto ilit1 = lit(fanin_array[1]);
	auto ilit2 = lit(fanin_array[2]);
	auto ilit3 = lit(fanin_array[3]);
	mSolver.add_norgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_norgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = lit(fanin_array[0]);
      auto ilit1 = lit(fanin_array[1]);
      mSolver.add_xorgate( olit, ilit0, ilit1);
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      auto ilit0 = lit(fanin_array[0]);
      auto ilit1 = lit(fanin_array[1]);
      mSolver.add_xnorgate( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
GateEnc::lit(
  const TpgNode* node
)
{
  return mVarMap(node);
}

END_NAMESPACE_DRUID
