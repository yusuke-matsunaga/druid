
/// @file TpgGateInfo.cc
/// @brief TpgGateInfo の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgGateInfo.h"
#include "SimpleGateInfo.h"
#include "CplxGateInfo.h"
#include "Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// 論理式中の演算子の数を数える．
SizeType
count_expr(
  const Expr& expr
)
{
  if ( !expr.is_op() ) {
    return 0;
  }

  SizeType n = 1;
  for ( auto subexpr: expr.operand_list() ) {
    n += count_expr(subexpr);
  }
  return n;
}

// 追加で必要なノード数を数える．
SizeType
extra_node_count(
  SizeType ni,
  const Expr& expr
)
{
  // まず入力部分に挿入されるノード数を数える．
  SizeType n = 0;
  for ( SizeType i: Range(ni) ) {
    SizeType p_num = expr.literal_num(i, false);
    SizeType n_num = expr.literal_num(i, true);
    ASSERT_COND( p_num > 0 || n_num > 0 );
    if ( n_num == 0 ) {
      if ( p_num > 1 ) {
	n += 1;
      }
    }
    else { // n_num > 0
      if ( p_num > 0 ) {
	n += 2;
      }
      else {
	n += 1;
      }
    }
  }

  // 次に論理式自体で必要となるノード数を数える．
  // ただし，根のノードはカウント済みなので1つ減らす．
  n += count_expr(expr) - 1;

  return n;
}

// calc_c_val の下請け関数
Val3
ccv_sub(
  const Expr& expr,
  const vector<Val3>& ivals
)
{
  if ( expr.is_zero() ) {
    return Val3::_0;
  }
  if ( expr.is_one() ) {
    return Val3::_1;
  }
  if ( expr.is_posi_literal() ) {
    int iid = expr.varid();
    return ivals[iid];
  }
  if ( expr.is_nega_literal() ) {
    SizeType iid = expr.varid();
    return ~ivals[iid];
  }

  if ( expr.is_and() ) {
    bool has_x = false;
    for ( auto expr1: expr.operand_list() ) {
      Val3 ival = ccv_sub(expr1, ivals);
      if ( ival == Val3::_0 ) {
	return Val3::_0;
      }
      if ( ival == Val3::_X ) {
	has_x = true;
      }
    }
    if ( has_x ) {
      return Val3::_X;
    }
    return Val3::_1;
  }

  if ( expr.is_or() ) {
    bool has_x = false;
    for ( auto expr1: expr.operand_list() ) {
      Val3 ival = ccv_sub(expr1, ivals);
      if ( ival == Val3::_1 ) {
	return Val3::_1;
      }
      if ( ival == Val3::_X ) {
	has_x = true;
      }
    }
    if ( has_x ) {
      return Val3::_X;
    }
    return Val3::_0;
  }

  if ( expr.is_xor() ) {
    Val3 val = Val3::_0;
    for ( auto expr1: expr.operand_list() ) {
      Val3 ival = ccv_sub(expr1, ivals);
      if ( ival == Val3::_X ) {
	return Val3::_X;
      }
      val = val ^ ival;
    }
    return val;
  }

  ASSERT_NOT_REACHED;
  return Val3::_X;
}

// 制御値の計算を行う．
Val3
calc_c_val(
  SizeType ni,
  const Expr& expr,
  SizeType ipos,
  Val3 val
)
{
  vector<Val3> ivals(ni, Val3::_X);
  ivals[ipos] = val;
  return ccv_sub(expr, ivals);
}

// 制御値の計算を行う．
Val3
c_val(
  PrimType gate_type,
  Val3 ival
)
{
  switch ( gate_type ) {
  case PrimType::C0:
  case PrimType::C1:
    // 常に X
    return Val3::_X;

  case PrimType::Buff:
    // そのまま返す．
    return ival;

  case PrimType::Not:
    // 反転して返す．
    return ~ival;

  case PrimType::And:
    // 0 の時のみ 0
    return ival == Val3::_0 ? Val3::_0 : Val3::_X;

  case PrimType::Nand:
    // 0 の時のみ 1
    return ival == Val3::_0 ? Val3::_1 : Val3::_X;

  case PrimType::Or:
    // 1 の時のみ 1
    return ival == Val3::_1 ? Val3::_1 : Val3::_X;

  case PrimType::Nor:
    // 1 の時のみ 0
    return ival == Val3::_1 ? Val3::_0 : Val3::_X;

  case PrimType::Xor:
  case PrimType::Xnor:
    // 常に X
    return Val3::_X;

  default:
    break;
  }
  ASSERT_NOT_REACHED;
  return Val3::_X;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス SimpleGateInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimpleGateInfo::SimpleGateInfo(
  PrimType gate_type
) : mGateType{gate_type}
{
  mCVal[0] = c_val(gate_type, Val3::_0);
  mCVal[1] = c_val(gate_type, Val3::_1);
}

// @brief デストラクタ
SimpleGateInfo::~SimpleGateInfo()
{
}

// @brief 組み込みタイプのときに true を返す．
bool
SimpleGateInfo::is_simple() const
{
  return true;
}

// @brief ゲートタイプを返す．
PrimType
SimpleGateInfo::gate_type() const
{
  return mGateType;
}

// @brief 論理式を返す．
Expr
SimpleGateInfo::expr() const
{
  // ダミー
  return Expr::make_invalid();
}

// @brief 追加ノード数を返す．
SizeType
SimpleGateInfo::extra_node_num() const
{
  return 0;
}

// @brief 制御値を返す．
Val3
SimpleGateInfo::cval(
  SizeType pos,
  Val3 val
) const
{
  int bval = (val == Val3::_0) ? 0 : 1;
  return mCVal[bval];
}


//////////////////////////////////////////////////////////////////////
// クラス CplxGateInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CplxGateInfo::CplxGateInfo(
  SizeType ni,
  const Expr& expr
) : mExpr{expr},
    mCVal(ni * 2)
{
  mExtraNodeNum = extra_node_count(ni, expr);
  for ( SizeType i: Range(ni) ) {
    mCVal[i * 2 + 0] = calc_c_val(ni, expr, i, Val3::_0);
    mCVal[i * 2 + 1] = calc_c_val(ni, expr, i, Val3::_1);
  }
}

// @brief デストラクタ
CplxGateInfo::~CplxGateInfo()
{
}

// @brief 組み込みタイプのときに true を返す．
bool
CplxGateInfo::is_simple() const
{
  return false;
}

// @brief ゲートタイプを返す．
PrimType
CplxGateInfo::gate_type() const
{
  // ダミー
  return PrimType::None;
}

// @brief 論理式を返す．
Expr
CplxGateInfo::expr() const
{
  return mExpr;
}

// @brief 追加ノード数を返す．
SizeType
CplxGateInfo::extra_node_num() const
{
  return mExtraNodeNum;
}

// @brief 制御値を返す．
Val3
CplxGateInfo::cval(
  SizeType pos,
  Val3 val
) const
{
  int bval = (val == Val3::_0) ? 0 : 1;
  return mCVal[pos * 2 + bval];
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGateInfoMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgGateInfoMgr::TpgGateInfoMgr()
{
  mSimpleType[0] = new SimpleGateInfo(PrimType::C0);
  mSimpleType[1] = new SimpleGateInfo(PrimType::C1);
  mSimpleType[2] = new SimpleGateInfo(PrimType::Buff);
  mSimpleType[3] = new SimpleGateInfo(PrimType::Not);
  mSimpleType[4] = new SimpleGateInfo(PrimType::And);
  mSimpleType[5] = new SimpleGateInfo(PrimType::Nand);
  mSimpleType[6] = new SimpleGateInfo(PrimType::Or);
  mSimpleType[7] = new SimpleGateInfo(PrimType::Nor);
  mSimpleType[8] = new SimpleGateInfo(PrimType::Xor);
  mSimpleType[9] = new SimpleGateInfo(PrimType::Xnor);
}

// @brief デストラクタ
TpgGateInfoMgr::~TpgGateInfoMgr()
{
  for ( int i: {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} ) {
    delete mSimpleType[i];
  }
  for ( auto p: mList ) {
    delete p;
  }
}

// @brief TpgGateInfo を登録する．
const TpgGateInfo*
TpgGateInfoMgr::new_info(
  SizeType ni,
  const Expr& expr
)
{
  auto prim_type = expr.analyze();
  if ( prim_type != PrimType::None ) {
    return simple_type(prim_type);
  }
  else {
    return complex_type(ni, expr);
  }
}

// @brief 組み込み型のオブジェクトを返す．
const TpgGateInfo*
TpgGateInfoMgr::simple_type(
  PrimType prim_type
)
{
  switch ( prim_type ) {
  case PrimType::C0:   return mSimpleType[0];
  case PrimType::C1:   return mSimpleType[1];
  case PrimType::Buff: return mSimpleType[2];
  case PrimType::Not:  return mSimpleType[3];
  case PrimType::And:  return mSimpleType[4];
  case PrimType::Nand: return mSimpleType[5];
  case PrimType::Or:   return mSimpleType[6];
  case PrimType::Nor:  return mSimpleType[7];
  case PrimType::Xor:  return mSimpleType[8];
  case PrimType::Xnor: return mSimpleType[9];
  default: break;
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 複合型のオブジェクトを返す．
const TpgGateInfo*
TpgGateInfoMgr::complex_type(
  SizeType ni,
  const Expr& expr
)
{
  auto node_info = new CplxGateInfo(ni, expr);
  mList.push_back(node_info);
  return node_info;
}

END_NAMESPACE_DRUID
