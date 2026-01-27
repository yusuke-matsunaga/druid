
/// @file BitVector.cc
/// @brief BitVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/BitVector.h"
#include "BitVectorRep.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
BitVector::BitVector(
  SizeType len
) : mPtr{BitVectorRep::new_vector(len)}
{
}

// @brief コピーコンストラクタ
BitVector::BitVector(
  const BitVector& src
) : mPtr{src.mPtr}
{
}

// @brief コピー代入演算子
BitVector&
BitVector::operator=(
  const BitVector& src
)
{
  mPtr = src.mPtr;

  return *this;
}

// @brief 2進文字列からオブジェクトを作る．
BitVector
BitVector::from_bin(
  const std::string& bin_str
)
{
  auto l = bin_str.size();
  auto bv = BitVector(l);
  if ( bv.set_from_bin(bin_str) ) {
    return bv;
  }
  // エラーの場合
  return BitVector(0);
}

// @brief HEX文字列からオブジェクトを作る．
BitVector
BitVector::from_hex(
  SizeType len,
  const std::string& hex_str
)
{
  auto bv = BitVector(len);
  if ( bv.set_from_hex(hex_str) ) {
    return bv;
  }
  // エラーの場合
  return BitVector(0);
}

// @brief デストラクタ
BitVector::~BitVector()
{
}

// @brief ベクタ長を返す．
SizeType
BitVector::len() const
{
  return mPtr->len();
}

// @brief 値を得る．
Val3
BitVector::val(
  SizeType pos
) const
{
  return mPtr->val(pos);
}

// @brief X の個数を得る．
SizeType
BitVector::x_count() const
{
  return mPtr->x_count();
}

// @brief マージして代入する．
BitVector&
BitVector::operator&=(
  const BitVector& right
)
{
  uniquefy();

  mPtr->merge(*right.mPtr);

  return *this;
}

// @brief 内容を BIN 形式で表す．
std::string
BitVector::bin_str() const
{
  return mPtr->bin_str();
}

// @brief 内容を HEX 形式で表す．
std::string
BitVector::hex_str() const
{
  return mPtr->hex_str();
}

// @brief すべて未定(X) で初期化する．
void
BitVector::init()
{
  uniquefy();

  mPtr->init();
}

// @brief 1ビット左シフトを行う．
void
BitVector::lshift(
  Val3 new_val
)
{
  uniquefy();

  mPtr->lshift(new_val);
}

// @brief 1ビット右シフトを行う．
void
BitVector::rshift(
  Val3 new_val
)
{
  uniquefy();

  mPtr->rshift(new_val);
}

// @brief 値を設定する．
void
BitVector::set_val(
  SizeType pos,
  Val3 val
)
{
  uniquefy();

  mPtr->set_val(pos, val);
}

// @brief BIN文字列から内容を設定する．
bool
BitVector::set_from_bin(
  const std::string& bin_string
)
{
  uniquefy();

  return mPtr->set_from_bin(bin_string);
}

// @brief HEX文字列から内容を設定する．
bool
BitVector::set_from_hex(
  const std::string& hex_string
)
{
  uniquefy();

  return mPtr->set_from_hex(hex_string);
}

// @brief 両立関係の比較を行う．
bool
BitVector::operator&&(
  const BitVector& right  ///< [in] オペランド2
) const
{
  return BitVectorRep::is_compat(*mPtr, *right.mPtr);
}

// @brief 等価関係の比較を行なう．
bool
BitVector::operator==(
  const BitVector& right
) const
{
  return BitVectorRep::is_eq(*mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
bool
BitVector::operator<(
  const BitVector& right ///< [in] オペランド2
) const
{
  return BitVectorRep::is_lt(*mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
bool
BitVector::operator<=(
  const BitVector& right
) const
{
  return BitVectorRep::is_le(*mPtr, *right.mPtr);
}

// @brief ハッシュ値を計算する．
SizeType
BitVector::hash() const
{
  return mPtr->hash();
}

// @brief 多重参照の場合に複製して単一参照にする．
void
BitVector::uniquefy()
{
  if ( mPtr.use_count() > 1) {
    // 内容を変更するので複製する．
    mPtr = BitVectorRep::new_vector(*mPtr);
  }
}

// @brief 値をセットする．
void
BitVector::set_block(
  SizeType pos,
  PackedVal v0,
  PackedVal v1
)
{
  mPtr->set_block(pos, v0, v1);
}

// @brief X の部分に値をセットする．
void
BitVector::fix_block(
  SizeType pos,
  PackedVal v0,
  PackedVal v1
)
{
  mPtr->fix_block(pos, v0, v1);
}

END_NAMESPACE_DRUID
