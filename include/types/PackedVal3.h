#ifndef PACKEDVAL3_H
#define PACKEDVAL3_H

/// @file PackedVal3.h
/// @brief 2ワードにパックした3値のビットベクタ型の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "PackedVal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class PackedVal3 PackedVal3.h "PackedVal3.h"
/// @brief PackedVal2つで3値のビットベクタを表すクラス
//////////////////////////////////////////////////////////////////////
class PackedVal3
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 不定値になる．
  PackedVal3(
  ) : mVal0{PV_ALL0},
      mVal1{PV_ALL0}
  {
  }

  /// @brief コピーコンストラクタ
  PackedVal3(
    const PackedVal3& src
  ) = default;

  /// @brief コンストラクタ
  ///
  /// val0 と val1 の両方のビットが1になったら不正
  PackedVal3(
    PackedVal val0, ///< [in] 0を表すビットベクタ
    PackedVal val1  ///< [in] 1を表すビットベクタ
  )
  {
    set(val0, val1);
  }

  /// @brief 2値の PackedVal からの変換コンストラクタ
  explicit
  PackedVal3(
    PackedVal val  ///< [in] 値
  ) : mVal0{~val},
      mVal1{ val}
  {
  }

  /// @brief デストラクタ
  ~PackedVal3() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @grief 0 のワードを取り出す．
  PackedVal
  val0() const
  {
    return mVal0;
  }

  /// @brief 1 のワードを取り出す．
  PackedVal
  val1() const
  {
    return mVal1;
  }

  /// @brief 0|1 か X かを区別するワードを取り出す．
  ///
  /// 1のビットはもとの値が0か1
  PackedVal
  val01() const
  {
    return mVal0 | mVal1;
  }

  /// @brief 2値の代入演算子
  /// @return 代入後の自身への参照を返す．
  PackedVal3&
  operator=(
    PackedVal val  ///< [in] 値
  )
  {
    mVal0 = ~val;
    mVal1 =  val;
    return *this;
  }

  /// @brief 普通の代入演算子
  /// @return 代入後の自身への参照を返す．
  PackedVal3&
  operator=(
    const PackedVal3& val  ///< [in] 値
  ) = default;

  /// @brief 値をセットする．
  void
  set(
    PackedVal val0, ///< [in] 値0
    PackedVal val1  ///< [in] 値1
  )
  {
    // 両方が1のビットは不定値(X)にする．
    mVal0 = val0 & ~val1;
    mVal1 = val1 & ~val0;
  }

  /// @brief マスク付きで値をセットする．
  void
  set_with_mask(
    PackedVal3 val, ///< [in] 値
    PackedVal mask  ///< [in] マスク
  )
  {
    mVal0 &= ~mask;
    mVal0 |= val.mVal0 & mask;
    mVal1 &= ~mask;
    mVal1 |= val.mVal1 & mask;
}

  /// @brief マスク付きで値をセットする．
  void
  set_with_mask(
    PackedVal val,  ///< [in] 値
    PackedVal mask  ///< [in] マスク
  )
  {
    mVal0 &= ~mask;
    mVal0 |= (~val & mask);
    mVal1 &= ~mask;
    mVal1 |= ( val & mask);
  }

  /// @brief 自身を否定する演算
  /// @return 演算後の自身の参照を返す．
  PackedVal3&
  negate()
  {
    PackedVal tmp = mVal0;
    mVal0 = mVal1;
    mVal1 = tmp;

    return *this;
  }

  /// @brief AND付き代入
  /// @return 演算後の自身の参照を返す．
  PackedVal3&
  operator&=(
    PackedVal3 right  ///< [in] オペランド
  )
  {
    mVal0 |= right.mVal0;
    mVal1 &= right.mVal1;

    return *this;
  }

  /// @brief OR付き代入
  /// @return 演算後の自身の参照を返す．
  PackedVal3&
  operator|=(
    PackedVal3 right  ///< [in] オペランド
  )
  {
    mVal0 &= right.mVal0;
    mVal1 |= right.mVal1;

    return *this;
  }

  /// @brief XOR付き代入
  /// @return 演算後の自身の参照を返す．
  PackedVal3&
  operator^=(
    PackedVal3 right  ///< [in] オペランド
  )
  {
    auto tmp0_0 = mVal0 | right.mVal1;
    auto tmp0_1 = mVal1 & right.mVal0;

    auto tmp1_0 = mVal1 | right.mVal0;
    auto tmp1_1 = mVal0 & right.mVal1;

    mVal0 = tmp0_0 & tmp1_0;
    mVal1 = tmp0_1 | tmp1_1;

    return *this;
  }

  /// @brief XOR付き代入
  /// @return 演算後の自身の参照を返す．
  ///
  /// right が2値のバージョン
  PackedVal3&
  operator^=(
    PackedVal right  ///< [in] オペランド
  )
  {
    auto tmp_val0 = val0();
    auto tmp_val1 = val1();

    mVal0 &= ~right;
    mVal0 |= tmp_val1 & right;
    mVal1 &= ~right;
    mVal1 |= tmp_val0 & right;

    return *this;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 0のワード
  PackedVal mVal0;

  // 1のワード
  PackedVal mVal1;

};


//////////////////////////////////////////////////////////////////////
// PackedVal3 の演算
//////////////////////////////////////////////////////////////////////

/// @relates PackedVal3
/// @brief 比較演算子 (EQ)
inline
bool
operator==(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal3 right ///< [in] オペランド2
)
{
  return left.val0() == right.val0() && left.val1() == right.val1();
}

/// @relates PackedVal3
/// @brief 比較演算子 (NE)
inline
bool
operator!=(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal3 right ///< [in] オペランド2
)
{
  return !operator==(left, right);
}

/// @relates PackedVal3
/// @brief 否定演算
inline
PackedVal3
operator~(
  PackedVal3 right ///< [in] オペランド
)
{
  return PackedVal3{right.val1(), right.val0()};
}

/// @relates PackedVal3
/// @brief AND演算
inline
PackedVal3
operator&(
  PackedVal3 left,  ///< [in] オペランド1
  PackedVal3 right  ///< [in] オペランド2
)
{
  auto val0 = left.val0() | right.val0();
  auto val1 = left.val1() & right.val1();
  return PackedVal3{val0, val1};
}

/// @relates PackedVal3
/// @brief OR演算
inline
PackedVal3
operator|(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal3 right ///< [in] オペランド2
)
{
  auto val0 = left.val0() & right.val0();
  auto val1 = left.val1() | right.val1();
  return PackedVal3{val0, val1};
}

/// @relates PackedVal3
/// @brief XOR演算
inline
PackedVal3
operator^(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal3 right ///< [in] オペランド2
)
{
  auto tmp0_0 = left.val0() | right.val1();
  auto tmp0_1 = left.val1() & right.val0();

  auto tmp1_0 = left.val1() | right.val0();
  auto tmp1_1 = left.val0() & right.val1();

  auto val0 = tmp0_0 & tmp1_0;
  auto val1 = tmp0_1 | tmp1_1;

  return PackedVal3{val0, val1};
}

/// @relates PackedVal3
/// @brief XOR演算
///
/// right が2値のバージョン
inline
PackedVal3
operator^(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal right  ///< [in] オペランド2
)
{
  auto tmp_val0 = left.val0();
  auto tmp_val1 = left.val1();

  auto val0 = (tmp_val0 & ~right) | (tmp_val1 &  right);
  auto val1 = (tmp_val0 &  right) | (tmp_val1 & ~right);

  return PackedVal3{val0, val1};
}

/// @relates PackedVal3
/// @brief DIFF演算
///
/// どちらかが 0 で他方が 1 のビットに1を立てたビットベクタを返す．
inline
PackedVal
diff(
  PackedVal3 left, ///< [in] オペランド1
  PackedVal3 right ///< [in] オペランド2
)
{
  auto val0_0 = left.val0();
  auto val0_1 = left.val1();
  auto val1_0 = right.val0();
  auto val1_1 = right.val1();

  return (val0_0 & ~val0_1 & ~val1_0 & val1_1) | (~val0_0 & val0_1 & val1_0 & ~val1_1);
}

END_NAMESPACE_DRUID

#endif // PACKEDVAL_H
