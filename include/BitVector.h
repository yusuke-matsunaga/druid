#ifndef BITVECTOR_H
#define BITVECTOR_H

/// @file BitVector.h
/// @brief BitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BitVectorRep.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BitVector BitVector.h "BitVector.h"
/// @brief 入力用のビットベクタを表すクラス
///
/// 実体は BitVectorRep が持つ．
//////////////////////////////////////////////////////////////////////
class BitVector
{
public:

  /// @brief コンストラクタ
  ///
  /// 内容は X で初期化される．
  explicit
  BitVector(
    SizeType len = 0  ///< [in] ベクタ長
  ) : mPtr{BitVectorRep::new_vector(len)}
  {
  }

  /// @brief コピーコンストラクタ
  BitVector(
    const BitVector& src  ///< [in] コピー元のソース
  ) : mPtr{src.mPtr}
  {
  }

  /// @brief コピー代入演算子
  BitVector&
  operator=(
    const BitVector& src  ///< [in] コピー元のソース
  )
  {
    mPtr = src.mPtr;

    return *this;
  }

  /// @brief 2進文字列からオブジェクトを作る．
  ///
  /// - ベクタ長は文字列の長さから得る．
  /// - 文字列は '0', '1', 'X', 'x' で構成される．
  /// - 最初の文字が0ビット目となる．
  /// - bin_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_bin_str(
    const string& bin_str  ///< [in] 2進文字列
  )
  {
    SizeType l = bin_str.size();
    BitVector bv{l};
    if ( bv.set_from_bin(bin_str) ) {
      return bv;
    }
    // エラーの場合
    return BitVector(0);
  }

  /// @brief HEX文字列からオブジェクトを作る．
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - hex_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_hex_str(
    SizeType len,         ///< [in] ベクタ長
    const string& hex_str ///< [in] HEX文字列
  )
  {
    BitVector bv{len};
    if ( bv.set_from_hex(hex_str) ) {
      return bv;
    }
    // エラーの場合
    return BitVector(0);
  }

  /// @brief デストラクタ
  ~BitVector() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  SizeType
  len() const
  {
    return mPtr->len();
  }

  /// @brief 値を得る．
  Val3
  val(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < len() )
  ) const
  {
    return mPtr->val(pos);
  }

  /// @brief X の個数を得る．
  SizeType
  x_count() const
  {
    return mPtr->x_count();
  }

  /// @brief マージして代入する．
  BitVector&
  operator&=(
    const BitVector& right
  )
  {
    uniquefy();

    mPtr->merge(*right.mPtr);

    return *this;
  }

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const
  {
    return mPtr->bin_str();
  }

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const
  {
    return mPtr->hex_str();
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init()
  {
    uniquefy();

    mPtr->init();
  }

  /// @brief 値を設定する．
  void
  set_val(
    SizeType pos, ///< [in] 位置番号 ( 0 <= pos < len() )
    Val3 val      ///< [in] 値
  )
  {
    uniquefy();

    mPtr->set_val(pos, val);
  }

  /// @brief BIN文字列から内容を設定する．
  /// @retval true 適切に設定された．
  /// @retval false bin_string に不適切な文字が含まれていた．
  ///
  /// - bin_string がベクタ長より短い時には残りはXで初期化される．
  /// - bin_string がベクタ長より長い時には余りは切り捨てられる．
  /// - 有効な文字は '0', '1', 'x', 'X'
  bool
  set_from_bin(
    const string& bin_string  ///< [in] BIN文字列
  )
  {
    uniquefy();

    return mPtr->set_from_bin(bin_string);
  }

  /// @brief HEX文字列から内容を設定する．
  /// @retval true 適切に設定された．
  /// @retval false hex_string に不適切な文字が含まれていた．
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - この形式は X を扱えない．
  bool
  set_from_hex(
    const string& hex_string  ///< [in] HEX 文字列
  )
  {
    uniquefy();

    return mPtr->set_from_hex(hex_string);
  }

  /// @brief 乱数パタンを設定する．
  ///
  /// - 結果はかならず 0 か 1 になる．(Xは含まれない)
  template<class URNG>
  void
  set_from_random(
    URNG& randgen  ///< [in] 乱数生成器
  )
  {
    uniquefy();

    mPtr->set_from_random(randgen);
  }

  /// @brief X の部分を乱数で 0/1 に設定する．
  template<class URNG>
  void
  fix_x_from_random(
    URNG& randgen  ///< [in] 乱数生成器
  )
  {
    uniquefy();

    mPtr->fix_x_from_random(randgen);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 2高演算
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立関係の比較を行う．
  /// @return left と right が両立する時 true を返す．
  bool
  operator&&(
    const BitVector& right  ///< [in] オペランド2
  ) const
  {
    return BitVectorRep::is_compat(*mPtr, *right.mPtr);
  }

  /// @brief 等価関係の比較を行なう．
  /// @return left と right が等しいとき true を返す．
  bool
  operator==(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return BitVectorRep::is_eq(*mPtr, *right.mPtr);
  }

  /// @brief 等価関係の比較を行なう．
  /// @return left と right が等しくないとき true を返す．
  bool
  operator!=(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return !operator==(right);
  }

  /// @brief 包含関係の比較を行なう
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - false だからといって逆に left が right を含むとは限らない．
  bool
  operator<(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return BitVectorRep::is_lt(*mPtr, *right.mPtr);
  }

  /// @brief 包含関係の比較を行なう．
  /// @return minterm の集合として left が right を含んでいたら true を返す．
  ///
  /// - false だからといって逆に right が left を含むとは限らない．
  bool
  operator>(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return right.operator<(*this);
  }

  /// @brief 包含関係の比較を行なう
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に left が right を含むとは限らない．
  bool
  operator<=(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return BitVectorRep::is_le(*mPtr, *right.mPtr);
  }

  /// @brief 包含関係の比較を行なう
  /// @return minterm の集合として left が right を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に right が left を含むとは限らない．
  bool
  operator>=(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return right.operator<=(*this);
  }

  /// @brief マージする．
  /// @return マージ結果を返す．
  ///
  /// left と right がコンフリクトしている時の結果は不定
  BitVector
  operator&(
    const BitVector& right ///< [in] オペランド2
  ) const
  {
    return BitVector(*this).operator&=(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 多重参照の場合に複製して単一参照にする．
  ///
  /// 内容を書き換える前に呼ばれる．
  void
  uniquefy()
  {
    if ( !mPtr.unique() ) {
      // 内容を変更するので複製する．
      mPtr = std::shared_ptr<BitVectorRep>(BitVectorRep::new_vector(*mPtr));
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::shared_ptr<BitVectorRep> mPtr;

};

#if 0
//////////////////////////////////////////////////////////////////////
// BitVector の演算
//////////////////////////////////////////////////////////////////////

/// @relates BitVector
/// @brief 両立関係の比較を行う．
/// @return left と right が両立する時 true を返す．
inline
bool
operator&&(
  const BitVector& left,  ///< [in] オペランド1
  const BitVector& right  ///< [in] オペランド2
)
{
  return BitVectorRep::is_compat(*left.mPtr, *right.mPtr);
}

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @return left と right が等しいとき true を返す．
inline
bool
operator==(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return BitVectorRep::is_eq(*left.mPtr, *right.mPtr);
}

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return !operator==(left, right);
}

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator<(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return BitVectorRep::is_lt(*left.mPtr, *right.mPtr);
}


/// @relates BitVector
/// @brief 包含関係の比較を行なう．
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return operator<(right, left);
}

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator<=(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return BitVectorRep::is_le(*left.mPtr, *right.mPtr);
}

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>=(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return operator<=(right, left);
}

/// @relates BitVector
/// @brief マージする．
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
inline
BitVector
operator&(
  const BitVector& left, ///< [in] オペランド1
  const BitVector& right ///< [in] オペランド2
)
{
  return BitVector(left).operator&=(right);
}
#endif

END_NAMESPACE_DRUID

#endif // BITVECTOR_H
