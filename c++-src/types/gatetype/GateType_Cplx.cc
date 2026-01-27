
/// @file GateType_Cplx.cc
/// @brief GateType_Cplx の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType_Cplx.h"
#include "types/Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateType
//////////////////////////////////////////////////////////////////////

// @brief 複合型のオブジェクトを作る．
GateType*
GateType::new_cplx(
  SizeType id,
  SizeType input_num,
  const Expr& expr
)
{
  auto prim_type = expr.analyze();
  if ( prim_type != PrimType::None ) {
    return new_primitive(id, input_num, prim_type);
  }
  else {
    return new GateType_Cplx(id, input_num, expr);
  }
}


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
    auto p_num = expr.literal_num(i, false);
    auto n_num = expr.literal_num(i, true);
    if ( p_num == 0 && n_num == 0 ) {
      throw std::logic_error{"p_num == 0 && n_num == 0"};
    }
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
  const std::vector<Val3>& ivals
)
{
  if ( expr.is_zero() ) {
    return Val3::_0;
  }
  if ( expr.is_one() ) {
    return Val3::_1;
  }
  if ( expr.is_positive_literal() ) {
    auto iid = expr.varid();
    return ivals[iid];
  }
  if ( expr.is_negative_literal() ) {
    auto iid = expr.varid();
    return ~ivals[iid];
  }

  if ( expr.is_and() ) {
    bool has_x = false;
    for ( auto expr1: expr.operand_list() ) {
      auto ival = ccv_sub(expr1, ivals);
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
    auto has_x = false;
    for ( auto expr1: expr.operand_list() ) {
      auto ival = ccv_sub(expr1, ivals);
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
    auto val = Val3::_0;
    for ( auto expr1: expr.operand_list() ) {
      auto ival = ccv_sub(expr1, ivals);
      if ( ival == Val3::_X ) {
	return Val3::_X;
      }
      val = val ^ ival;
    }
    return val;
  }

  throw std::logic_error{"Never happen"};
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
  std::vector<Val3> ivals(ni, Val3::_X);
  ivals[ipos] = val;
  return ccv_sub(expr, ivals);
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス GateType_Cplx
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateType_Cplx::GateType_Cplx(
  SizeType id,
  SizeType ni,
  const Expr& expr
) : GateType(id),
    mExpr{expr},
    mCVal(ni * 2)
{
  mExtraNodeNum = extra_node_count(ni, expr);
  for ( SizeType i: Range(ni) ) {
    mCVal[i * 2 + 0] = calc_c_val(ni, expr, i, Val3::_0);
    mCVal[i * 2 + 1] = calc_c_val(ni, expr, i, Val3::_1);
  }
}

// @brief 入力数を返す．
SizeType
GateType_Cplx::input_num() const
{
  return mCVal.size() / 2;
}

// @brief 論理式タイプのときに true を返す．
bool
GateType_Cplx::is_expr() const
{
  return true;
}

// @brief 論理式を返す．
Expr
GateType_Cplx::expr() const
{
  return mExpr;
}

// @brief 追加ノード数を返す．
SizeType
GateType_Cplx::extra_node_num() const
{
  return mExtraNodeNum;
}

// @brief 制御値を返す．
Val3
GateType_Cplx::cval(
  SizeType pos,
  Val3 val
) const
{
  switch ( val ) {
  case Val3::_X: return Val3::_X;
  case Val3::_0: return mCVal[pos * 2 + 0];
  case Val3::_1: return mCVal[pos * 2 + 1];
  }
}

END_NAMESPACE_DRUID
