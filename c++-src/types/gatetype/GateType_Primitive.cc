
/// @file GateType_Primitive.cc
/// @brief GateType_Primitive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType_Primitive.h"
#include "types/Val3.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateType
//////////////////////////////////////////////////////////////////////

// @brief 組込み型のオブジェクトを作る．
GateType*
GateType::new_primitive(
  SizeType id,
  SizeType input_num,
  PrimType gate_type
)
{
  return new GateType_Primitive(id, input_num, gate_type);
}


BEGIN_NONAMESPACE

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

  throw std::logic_error{"Never happen"};
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス GateType_Primitive
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateType_Primitive::GateType_Primitive(
  SizeType id,
  SizeType input_num,
  PrimType prim_type
) : GateType(id),
    mInputNum{input_num},
    mPrimType{prim_type}
{
  mCVal[0] = c_val(prim_type, Val3::_0);
  mCVal[1] = c_val(prim_type, Val3::_1);
}

// @brief 入力数を返す．
SizeType
GateType_Primitive::input_num() const
{
  return mInputNum;
}

// @brief 組み込みタイプのときに true を返す．
bool
GateType_Primitive::is_primitive() const
{
  return true;
}

// @brief ゲートタイプを返す．
PrimType
GateType_Primitive::primitive_type() const
{
  return mPrimType;
}

// @brief 制御値を返す．
Val3
GateType_Primitive::cval(
  SizeType pos,
  Val3 val
) const
{
  switch ( val ) {
  case Val3::_X: return Val3::_X;
  case Val3::_0: return mCVal[0];
  case Val3::_1: return mCVal[1];
  }
}

END_NAMESPACE_DRUID
