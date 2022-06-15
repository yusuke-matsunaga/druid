
/// @file GateEnc.cc
/// @brief GateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "GateEnc.h"

#include "TpgNode.h"
#include "GateType.h"
#include "VidMap.h"

#include "ym/SatSolver.h"
#include "ym/SatTseitinEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
// @param[in] solver SATソルバ
// @param[in] varmap 変数番号のマップ
GateEnc::GateEnc(SatSolver& solver,
		 const VidMap& varmap) :
  mSolver(solver),
  mVarMap(varmap)
{
}

// @brief デストラクタ
GateEnc::~GateEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
void
GateEnc::make_cnf(const TpgNode* node)
{
  make_cnf(node, mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
GateEnc::make_cnf(const TpgNode* node,
		  SatLiteral olit)
{
  int ni = node->fanin_num();
  const auto& fanin_array = node->fanin_list();
  SatTseitinEnc enc{mSolver};
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
      SatLiteral ilit = lit(fanin_array[0]);
      enc.add_buffgate(ilit, olit);
    }
    break;

  case GateType::Not:
    {
      SatLiteral ilit = lit(fanin_array[0]);
      enc.add_notgate(ilit, olit);
    }
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	enc.add_andgate(olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	enc.add_andgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	enc.add_andgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	enc.add_andgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	enc.add_nandgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	enc.add_nandgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	enc.add_nandgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	enc.add_nandgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	enc.add_orgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	enc.add_orgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	enc.add_orgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	enc.add_orgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	enc.add_norgate( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	enc.add_norgate( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	enc.add_norgate( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	enc.add_norgate( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = lit(fanin_array[0]);
      SatLiteral ilit1 = lit(fanin_array[1]);
      enc.add_xorgate( olit, ilit0, ilit1);
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = lit(fanin_array[0]);
      SatLiteral ilit1 = lit(fanin_array[1]);
      enc.add_xnorgate( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
GateEnc::lit(const TpgNode* node)
{
  return SatLiteral(mVarMap(node));
}

END_NAMESPACE_DRUID
