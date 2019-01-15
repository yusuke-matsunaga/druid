#ifndef PACKEDVAL3_H
#define PACKEDVAL3_H

/// @file PackedVal3.h
/// @brief 2ワードにパックした3値のビットベクタ型の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
  PackedVal3();

  /// @brief コピーコンストラクタ
  PackedVal3(const PackedVal3& src) = default;

  /// @brief コンストラクタ
  /// @param[in] val0 0を表すビットベクタ
  /// @param[in] val1 1を表すビットベクタ
  ///
  /// val0 と val1 の両方のビットが1になったら不正
  PackedVal3(PackedVal val0,
	     PackedVal val1);

  /// @brief 2値の PackedVal からの変換コンストラクタ
  /// @param[in] val 値
  explicit
  PackedVal3(PackedVal val);

  /// @brief デストラクタ
  ~PackedVal3();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @grief 0 のワードを取り出す．
  PackedVal
  val0() const;

  /// @brief 1 のワードを取り出す．
  PackedVal
  val1() const;

  /// @brief 0|1 か X かを区別するワードを取り出す．
  ///
  /// 1のビットはもとの値が0か1
  PackedVal
  val01() const;

  /// @brief 2値の代入演算子
  /// @param[in] val 値
  /// @return 代入後の自身への参照を返す．
  const PackedVal3&
  operator=(PackedVal val);

  /// @brief 普通の代入演算子
  /// @param[in] val 値
  /// @return 代入後の自身への参照を返す．
  PackedVal3&
  operator=(const PackedVal3& val) = default;

  /// @brief 値をセットする．
  /// @param[in] val0, val1 値
  void
  set(PackedVal val0,
      PackedVal val1);

  /// @brief マスク付きで値をセットする．
  /// @param[in] val 値
  /// @param[in] mask
  void
  set_with_mask(PackedVal3 val,
		PackedVal mask);

  /// @brief マスク付きで値をセットする．
  /// @param[in] val 値
  /// @param[in] mask
  void
  set_with_mask(PackedVal val,
		PackedVal mask);

  /// @brief 自身を否定する演算
  /// @return 演算後の自身の参照を返す．
  const PackedVal3&
  negate();

  /// @brief AND付き代入
  /// @param[in] right オペランド
  /// @return 演算後の自身の参照を返す．
  const PackedVal3&
  operator&=(PackedVal3 right);

  /// @brief OR付き代入
  /// @param[in] right オペランド
  /// @return 演算後の自身の参照を返す．
  const PackedVal3&
  operator|=(PackedVal3 right);

  /// @brief XOR付き代入
  /// @param[in] right オペランド
  /// @return 演算後の自身の参照を返す．
  const PackedVal3&
  operator^=(PackedVal3 right);

  /// @brief XOR付き代入
  /// @param[in] right オペランド
  /// @return 演算後の自身の参照を返す．
  ///
  /// right が2値のバージョン
  const PackedVal3&
  operator^=(PackedVal right);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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
/// @param[in] left, right オペランド
bool
operator==(PackedVal3 left,
	   PackedVal3 right);

/// @relates PackedVal3
/// @brief 比較演算子 (NE)
/// @param[in] left, right オペランド
bool
operator!=(PackedVal3 left,
	   PackedVal3 right);

/// @relates PackedVal3
/// @brief 否定演算
/// @param[in] right オペランド
PackedVal3
operator~(PackedVal3 right);

/// @relates PackedVal3
/// @brief AND演算
/// @param[in] left, right オペランド
PackedVal3
operator&(PackedVal3 left,
	  PackedVal3 right);

/// @relates PackedVal3
/// @brief OR演算
/// @param[in] left, right オペランド
PackedVal3
operator|(PackedVal3 left,
	  PackedVal3 right);

/// @relates PackedVal3
/// @brief XOR演算
/// @param[in] left, right オペランド
PackedVal3
operator^(PackedVal3 left,
	  PackedVal3 right);

/// @relates PackedVal3
/// @brief XOR演算
/// @param[in] left, right オペランド
///
/// right が2値のバージョン
PackedVal3
operator^(PackedVal3 left,
	  PackedVal right);

/// @relates PackedVal3
/// @brief DIFF演算
/// @param[in] left, right オペランド
///
/// どちらかが 0 で他方が 1 のビットに1を立てたビットベクタを返す．
PackedVal
diff(PackedVal3 left,
     PackedVal3 right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
//
// 不定値になる．
inline
PackedVal3::PackedVal3() :
  mVal0(kPvAll0),
  mVal1(kPvAll0)
{
}

// @brief 2値のコンストラクタ
// @param[in] val 値
inline
PackedVal3::PackedVal3(PackedVal val) :
  mVal0(~val),
  mVal1( val)
{
}

// @brief コンストラクタ
// @param[in] val0, val1 値
inline
PackedVal3::PackedVal3(PackedVal val0,
		       PackedVal val1)
{
  set(val0, val1);
}

// @brief デストラクタ
inline
PackedVal3::~PackedVal3()
{
}

// @grief 0 のワードを取り出す．
inline
PackedVal
PackedVal3::val0() const
{
  return mVal0;
}

// @brief 1 のワードを取り出す．
inline
PackedVal
PackedVal3::val1() const
{
  return mVal1;
}

// @brief 0|1 か X かを区別するワードを取り出す．
//
// 1のビットはもとの値が0か1
inline
PackedVal
PackedVal3::val01() const
{
  return mVal0 | mVal1;
}

// @brief 2値の代入演算子
// @param[in] val 値
// @return 代入後の自身への参照を返す．
inline
const PackedVal3&
PackedVal3::operator=(PackedVal val)
{
  mVal0 = ~val;
  mVal1 =  val;
  return *this;
}

// @brief 値をセットする．
// @param[in] val0, val1 値
inline
void
PackedVal3::set(PackedVal val0,
		PackedVal val1)
{
  // 両方が1のビットは不定値(X)にする．
  mVal0 = val0 & ~val1;
  mVal1 = val1 & ~val0;
}

// @brief マスク付きで値をセットする．
// @param[in] val 値
// @param[in] mask
inline
void
PackedVal3::set_with_mask(PackedVal3 val,
			  PackedVal mask)
{
  mVal0 &= ~mask;
  mVal0 |= val.mVal0 & mask;
  mVal1 &= ~mask;
  mVal1 |= val.mVal1 & mask;
}

// @brief マスク付きで値をセットする．
// @param[in] val 値
// @param[in] mask
inline
void
PackedVal3::set_with_mask(PackedVal val,
			  PackedVal mask)
{
  mVal0 &= ~mask;
  mVal0 |= (~val & mask);
  mVal1 &= ~mask;
  mVal1 |= ( val & mask);
}

// @brief 自身を否定する演算
// @return 演算後の自身の参照を返す．
inline
const PackedVal3&
PackedVal3::negate()
{
  PackedVal tmp = mVal0;
  mVal0 = mVal1;
  mVal1 = tmp;

  return *this;
}

// @relates PackedVal3
// @brief 比較演算子 (EQ)
// @param[in] left, right オペランド
inline
bool
operator==(PackedVal3 left,
	   PackedVal3 right)
{
  return left.val0() == right.val0() && left.val1() == right.val1();
}

// @relates PackedVal3
// @brief 比較演算子 (NE)
// @param[in] left, right オペランド
inline
bool
operator!=(PackedVal3 left,
	   PackedVal3 right)
{
  return !operator==(left, right);
}

// @brief 否定演算
// @param[in] right オペランド
inline
PackedVal3
operator~(PackedVal3 right)
{
  return PackedVal3(right.val1(), right.val0());
}

// @brief AND付き代入
// @param[in] right オペランド
// @return 演算後の自身の参照を返す．
inline
const PackedVal3&
PackedVal3::operator&=(PackedVal3 right)
{
  mVal0 |= right.mVal0;
  mVal1 &= right.mVal1;

  return *this;
}

// @brief AND演算
// @param[in] left, right オペランド
inline
PackedVal3
operator&(PackedVal3 left,
	  PackedVal3 right)
{
  PackedVal val0 = left.val0() | right.val0();
  PackedVal val1 = left.val1() & right.val1();
  return PackedVal3(val0, val1);
}

// @brief OR付き代入
// @param[in] right オペランド
// @return 演算後の自身の参照を返す．
inline
const PackedVal3&
PackedVal3::operator|=(PackedVal3 right)
{
  mVal0 &= right.mVal0;
  mVal1 |= right.mVal1;

  return *this;
}

// @brief OR演算
// @param[in] left, right オペランド
inline
PackedVal3
operator|(PackedVal3 left,
	  PackedVal3 right)
{
  PackedVal val0 = left.val0() & right.val0();
  PackedVal val1 = left.val1() | right.val1();
  return PackedVal3(val0, val1);
}

// @brief XOR付き代入
// @param[in] right オペランド
// @return 演算後の自身の参照を返す．
inline
const PackedVal3&
PackedVal3::operator^=(PackedVal3 right)
{
  PackedVal tmp0_0 = mVal0 | right.mVal1;
  PackedVal tmp0_1 = mVal1 & right.mVal0;

  PackedVal tmp1_0 = mVal1 | right.mVal0;
  PackedVal tmp1_1 = mVal0 & right.mVal1;

  mVal0 = tmp0_0 & tmp1_0;
  mVal1 = tmp0_1 | tmp1_1;

  return *this;
}

// @brief XOR演算
// @param[in] left, right オペランド
inline
PackedVal3
operator^(PackedVal3 left,
	  PackedVal3 right)
{
  PackedVal tmp0_0 = left.val0() | right.val1();
  PackedVal tmp0_1 = left.val1() & right.val0();

  PackedVal tmp1_0 = left.val1() | right.val0();
  PackedVal tmp1_1 = left.val0() & right.val1();

  PackedVal val0 = tmp0_0 & tmp1_0;
  PackedVal val1 = tmp0_1 | tmp1_1;

  return PackedVal3(val0, val1);
}

// @brief XOR付き代入
// @param[in] right オペランド
// @return 演算後の自身の参照を返す．
//
// right が2値のバージョン
inline
const PackedVal3&
PackedVal3::operator^=(PackedVal right)
{
  PackedVal tmp_val0 = val0();
  PackedVal tmp_val1 = val1();

  mVal0 &= ~right;
  mVal0 |= tmp_val1 & right;
  mVal1 &= ~right;
  mVal1 |= tmp_val0 & right;

  return *this;
}

// @brief XOR演算
// @param[in] left, right オペランド
//
// right が2値のバージョン
inline
PackedVal3
operator^(PackedVal3 left,
	  PackedVal right)
{
  PackedVal tmp_val0 = left.val0();
  PackedVal tmp_val1 = left.val1();
  PackedVal val0 = (tmp_val0 & ~right) | (tmp_val1 &  right);
  PackedVal val1 = (tmp_val0 &  right) | (tmp_val1 & ~right);
  return PackedVal3(val0, val1);
}

// @brief DIFF演算
// @param[in] left, right オペランド
//
// どちらかが 0 で他方が 1 のビットに1を立てたビットベクタを返す．
inline
PackedVal
diff(PackedVal3 left,
     PackedVal3 right)
{
  PackedVal val0_0 = left.val0();
  PackedVal val0_1 = left.val1();
  PackedVal val1_0 = right.val0();
  PackedVal val1_1 = right.val1();

  return (val0_0 & ~val0_1 & ~val1_0 & val1_1) | (~val0_0 & val0_1 & val1_0 & ~val1_1);
}

END_NAMESPACE_DRUID

#endif // PACKEDVAL_H
