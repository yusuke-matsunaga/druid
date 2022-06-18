
/// @file FaultyGateEnc.cc
/// @brief FaultyGateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FaultyGateEnc.h"

#include "TpgNode.h"
#include "TpgFault.h"
#include "GateType.h"
#include "VidMap.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"
#include "ym/SatTseitinEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultyGateEnc::FaultyGateEnc(
  SatSolver& solver,
  const VidMap& varmap,
  const TpgFault* fault
) : mSolver{solver},
    mVarMap{varmap},
    mFault{fault}
{
}

// @brief デストラクタ
FaultyGateEnc::~FaultyGateEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
FaultyGateEnc::make_cnf()
{
  const TpgNode* node = mFault->tpg_onode();
  make_cnf(mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
void
FaultyGateEnc::make_cnf(
  SatLiteral olit
)
{
  int fval = mFault->val();
  if ( mFault->is_stem_fault() ) {
    // 出力の故障の場合，ゲートの種類は関係ない．
    if ( fval ) {
      // 1縮退故障
      mSolver.add_clause(olit);
    }
    else {
      // 0縮退故障
      mSolver.add_clause(~olit);
    }
    return;
  }

  // 入力の故障の場合
  // 該当の入力以外のファンインのリテラルのリストを作る．
  const TpgNode* node = mFault->tpg_onode();
  SizeType ni = node->fanin_num();
  const auto& fanin_array = node->fanin_list();
  vector<SatLiteral> ilits;
  ilits.reserve(ni - 1);
  SizeType fpos = mFault->tpg_pos();
  for ( int i: Range(ni) ) {
    if ( i != fpos ) {
      ilits.push_back(lit(fanin_array[i]));
    }
  }

  SatTseitinEnc enc{mSolver};
  switch ( node->gate_type() ) {
  case GateType::Const0:
  case GateType::Const1:
  case GateType::Input:
    ASSERT_NOT_REACHED;
    break;

  case GateType::Buff:
    if ( fval ) {
      // 入力の1縮退故障
      mSolver.add_clause(olit);
    }
    else {
      // 入力の0縮退故障
      mSolver.add_clause(~olit);
    }
    break;

  case GateType::Not:
    if ( fval ) {
      // 入力の1縮退故障
      mSolver.add_clause(~olit);
    }
    else {
      // 入力の0縮退故障
      mSolver.add_clause(olit);
    }
    break;

  case GateType::And:
    if ( fval == 0 ) {
      // 入力の0縮退故障
      mSolver.add_clause(~olit);
    }
    else {
      // 入力の1縮退故障
      // ilits の要素数が 1 でも正しく動く．
      enc.add_andgate( olit, ilits);
    }
    break;

  case GateType::Nand:
    if ( fval == 0 ) {
      // 入力の 0 縮退故障
      mSolver.add_clause(olit);
    }
    else {
      // 入力の1縮退故障
      // ilits の要素数が 1 でも正しく動く．
      enc.add_nandgate( olit, ilits);
    }
    break;

  case GateType::Or:
    if ( fval == 1 ) {
      // 入力の 1 縮退故障
      mSolver.add_clause(olit);
    }
    else {
      // 入力の 0 縮退故障
      // ilits の要素数が 1 でも正しく動く．
      enc.add_orgate( olit, ilits );
    }
    break;

  case GateType::Nor:
    if ( fval == 1 ) {
      // 入力の1縮退故障
      mSolver.add_clause(~olit);
    }
    else {
      // ilits の要素数が 1 でも正しく動く．
      enc.add_norgate( olit, ilits);
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    if ( fval ) {
      enc.add_notgate( olit, ilits[0] );
    }
    else {
      enc.add_buffgate( olit, ilits[0] );
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    if ( fval ) {
      enc.add_buffgate( olit, ilits[0] );
    }
    else {
      enc.add_notgate( olit, ilits[0] );
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
FaultyGateEnc::lit(
  const TpgNode* node
)
{
  return mVarMap(node);
}

END_NAMESPACE_DRUID
