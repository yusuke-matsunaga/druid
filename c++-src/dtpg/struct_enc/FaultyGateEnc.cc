﻿
/// @file FaultyGateEnc.cc
/// @brief FaultyGateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "FaultyGateEnc.h"

#include "TpgNode.h"
#include "TpgFault.h"
#include "VidMap.h"
#include "ym/Range.h"
#include "ym/SatSolver.h"


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
#if 0
  auto fval = mFault->val();
  if ( mFault->is_stem_fault() ) {
    // 出力の故障の場合，ゲートの種類は関係ない．
    switch ( fval ) {
    case Fval2::zero:
      // 0縮退故障
      mSolver.add_clause(~olit);
      break;
    case Fval2::one:
      // 1縮退故障
      mSolver.add_clause(olit);
      break;
    }
    return;
  }

  // 入力の故障の場合
  // 該当の入力以外のファンインのリテラルのリストを作る．
  auto node = mFault->tpg_onode();
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

  switch ( node->gate_type() ) {
  case PrimType::C0:
  case PrimType::C1:
  case PrimType::None:
    ASSERT_NOT_REACHED;
    break;

  case PrimType::Buff:
    switch ( fval ) {
    case Fval2::zero:
      // 入力の0縮退故障
      mSolver.add_clause(~olit);
      break;
    case Fval2::one:
      // 入力の1縮退故障
      mSolver.add_clause(olit);
      break;
    }
    break;

  case PrimType::Not:
    switch( fval ) {
    case Fval2::zero:
      // 入力の0縮退故障
      mSolver.add_clause(olit);
      break;
    case Fval2::one:
      // 入力の1縮退故障
      mSolver.add_clause(~olit);
      break;
    }
    break;

  case PrimType::And:
    switch ( fval ) {
    case Fval2::zero:
      // 入力の0縮退故障
      mSolver.add_clause(~olit);
      break;
    case Fval2::one:
      // 入力の1縮退故障
      // ilits の要素数が 1 でも正しく動く．
      mSolver.add_andgate( olit, ilits);
      break;
    }
    break;

  case PrimType::Nand:
    switch ( fval ) {
    case Fval2::zero:
      // 入力の 0 縮退故障
      mSolver.add_clause(olit);
      break;
    case Fval2::one:
      // 入力の1縮退故障
      // ilits の要素数が 1 でも正しく動く．
      mSolver.add_nandgate( olit, ilits);
      break;
    }
    break;

  case PrimType::Or:
    switch ( fval ) {
    case Fval2::zero:
      // 入力の 0 縮退故障
      // ilits の要素数が 1 でも正しく動く．
      mSolver.add_orgate( olit, ilits );
      break;
    case Fval2::one:
      // 入力の 1 縮退故障
      mSolver.add_clause(olit);
      break;
    }
    break;

  case PrimType::Nor:
    switch ( fval ) {
    case Fval2::zero:
      // 入力の 0 縮退故障
      // ilits の要素数が 1 でも正しく動く．
      mSolver.add_norgate( olit, ilits);
      break;
    case Fval2::one:
      // 入力の1縮退故障
      mSolver.add_clause(~olit);
      break;
    }
    break;

  case PrimType::Xor:
    ASSERT_COND( ni == 2 );
    switch ( fval ) {
    case Fval2::zero:
      mSolver.add_buffgate( olit, ilits[0] );
      break;
    case Fval2::one:
      mSolver.add_notgate( olit, ilits[0] );
      break;
    }
    break;

  case PrimType::Xnor:
    ASSERT_COND( ni == 2 );
    switch ( fval ) {
    case Fval2::zero:
      mSolver.add_notgate( olit, ilits[0] );
      break;
    case Fval2::one:
      mSolver.add_buffgate( olit, ilits[0] );
      break;
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
#else
#warning "TODO"
#endif
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
