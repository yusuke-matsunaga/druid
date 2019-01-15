#ifndef BITVECTOR_H
#define BITVECTOR_H

/// @file BitVector.h
/// @brief BitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


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
  /// @param[in] len ベクタ長
  ///
  /// 内容は X で初期化される．
  explicit
  BitVector(int len = 0);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  BitVector(const BitVector& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  BitVector&
  operator=(const BitVector& src);

  /// @brief 2進文字列からオブジェクトを作る．
  /// @param[in] bin_str 2進文字列
  ///
  /// - ベクタ長は文字列の長さから得る．
  /// - 文字列は '0', '1', 'X', 'x' で構成される．
  /// - 最初の文字が0ビット目となる．
  /// - bin_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_bin_str(const string& bin_str);

  /// @brief HEX文字列からオブジェクトを作る．
  /// @param[in] len ベクタ長
  /// @param[in] hex_str HEX文字列
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - hex_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_hex_str(int len,
	       const string& hex_str);

  /// @brief デストラクタ
  ~BitVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  int
  len() const;

  /// @brief 値を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < len() )
  Val3
  val(int pos) const;

  /// @brief X の個数を得る．
  int
  x_count() const;

  /// @brief マージして代入する．
  BitVector&
  operator&=(const BitVector& right);

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @brief 値を設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < len() )
  /// @param[in] val 値
  void
  set_val(int pos,
	  Val3 val);

  /// @brief BIN文字列から内容を設定する．
  /// @param[in] bin_string BIN文字列
  /// @retval true 適切に設定された．
  /// @retval false bin_string に不適切な文字が含まれていた．
  ///
  /// - bin_string がベクタ長より短い時には残りはXで初期化される．
  /// - bin_string がベクタ長より長い時には余りは切り捨てられる．
  /// - 有効な文字は '0', '1', 'x', 'X'
  bool
  set_from_bin(const string& bin_string);

  /// @brief HEX文字列から内容を設定する．
  /// @param[in] hex_string HEX 文字列
  /// @retval true 適切に設定された．
  /// @retval false hex_string に不適切な文字が含まれていた．
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - この形式は X を扱えない．
  bool
  set_from_hex(const string& hex_string);

  /// @brief 乱数パタンを設定する．
  /// @param[in] randgen 乱数生成器
  ///
  /// - 結果はかならず 0 か 1 になる．(Xは含まれない)
  template<class URNG>
  void
  set_from_random(URNG& randgen);

  /// @brief X の部分を乱数で 0/1 に設定する．
  /// @param[in] randgen 乱数生成器
  template<class URNG>
  void
  fix_x_from_random(URNG& randgen);


public:
  //////////////////////////////////////////////////////////////////////
  // friend 関数の定義(publicに意味はない)
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立関係の比較を行う．
  /// @param[in] left, right オペランド
  /// @return left と right が両立する時 true を返す．
  friend
  bool
  operator&&(const BitVector& left,
	     const BitVector& right);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] left, right オペランド
  /// @return left と right が等しいとき true を返す．
  friend
  bool
  operator==(const BitVector& left,
	     const BitVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<(const BitVector& left,
	    const BitVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<=(const BitVector& left,
	     const BitVector& right);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 多重参照の場合に複製して単一参照にする．
  ///
  /// 内容を書き換える前に呼ばれる．
  void
  uniquefy();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::shared_ptr<BitVectorRep> mPtr;

};


//////////////////////////////////////////////////////////////////////
// BitVector の演算
//////////////////////////////////////////////////////////////////////

/// @relates BitVector
/// @brief 両立関係の比較を行う．
/// @param[in] left, right オペランド
/// @return left と right が両立する時 true を返す．
bool
operator&&(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しいとき true を返す．
bool
operator==(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
bool
operator!=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<(const BitVector& left,
	  const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - false だからといって逆に right が left を含むとは限らない．
bool
operator>(const BitVector& left,
	  const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に right が left を含むとは限らない．
bool
operator>=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
BitVector
operator&(const BitVector& left,
	  const BitVector& right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] len ベクタ長
inline
BitVector::BitVector(int len) :
  mPtr(BitVectorRep::new_vector(len))
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
inline
BitVector::BitVector(const BitVector& src) :
  mPtr(src.mPtr)
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
inline
BitVector&
BitVector::operator=(const BitVector& src)
{
  mPtr = src.mPtr;

  return *this;
}

// @brief 2進文字列からオブジェクトを作る．
// @param[in] bin_str 2進文字列
//
// 文字列は '0', '1', 'X', 'x' で構成される．
// 最初の文字が0ビット目となる．
inline
BitVector
BitVector::from_bin_str(const string& bin_str)
{
  int l = bin_str.size();
  BitVector bv(l);
  if ( bv.set_from_bin(bin_str) ) {
    return bv;
  }
  // エラーの場合
  return BitVector(0);
}

// @brief HEX文字列からオブジェクトを作る．
// @param[in] len ベクタ長
// @param[in] hex_str HEX文字列
//
// - hex_string が短い時には残りは0で初期化される．
// - hex_string が長い時には余りは捨てられる．
// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
// - hex_str が不適切な場合には長さ0のベクタを返す．
inline
BitVector
BitVector::from_hex_str(int len,
			const string& hex_str)
{
  BitVector bv(len);
  if ( bv.set_from_hex(hex_str) ) {
    return bv;
  }
  // エラーの場合
  return BitVector(0);
}

// @brief デストラクタ
inline
BitVector::~BitVector()
{
}

// @brief ベクタ長を返す．
inline
int
BitVector::len() const
{
  return mPtr->len();
}

// @brief 値を得る．
// @param[in] pos 位置番号 ( 0 <= pos < len() )
inline
Val3
BitVector::val(int pos) const
{
  return mPtr->val(pos);
}

// @brief X の個数を得る．
inline
int
BitVector::x_count() const
{
  return mPtr->x_count();
}

// @brief マージして代入する．
inline
BitVector&
BitVector::operator&=(const BitVector& right)
{
  uniquefy();

  mPtr->merge(*right.mPtr);

  return *this;
}

// @brief 両立関係の比較を行う．
// @param[in] left, right オペランド
// @return left と right が両立する時 true を返す．
inline
bool
operator&&(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_compat(*left.mPtr, *right.mPtr);
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しいとき true を返す．
inline
bool
operator==(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_eq(*left.mPtr, *right.mPtr);
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const BitVector& left,
	   const BitVector& right)
{
  return !operator==(left, right);
}

// @brief 包含関係の比較を行なう．
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
//
// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator<(const BitVector& left,
	  const BitVector& right)
{
  return BitVectorRep::is_lt(*left.mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator>(const BitVector& left,
	  const BitVector& right)
{
  return operator<(right, left);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
//
// - こちらは等しい場合も含む．
// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator<=(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_le(*left.mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - こちらは等しい場合も含む．
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator>=(const BitVector& left,
	   const BitVector& right)
{
  return operator<=(right, left);
}

// @relates BitVector
// @brief マージする．
// @param[in] left, right オペランド
// @return マージ結果を返す．
//
// left と right がコンフリクトしている時の結果は不定
inline
BitVector
operator&(const BitVector& left,
	  const BitVector& right)
{
  return BitVector(left).operator&=(right);
}

// @brief 内容を BIN 形式で表す．
inline
string
BitVector::bin_str() const
{
  return mPtr->bin_str();
}

// @brief 内容を HEX 形式で表す．
// @note X を含む場合の出力は不定
inline
string
BitVector::hex_str() const
{
  return mPtr->hex_str();
}

// @brief すべて未定(X) で初期化する．
inline
void
BitVector::init()
{
  uniquefy();

  mPtr->init();
}

// @brief 値を設定する．
// @param[in] pos 位置番号 ( 0 <= pos < len() )
// @param[in] val 値
inline
void
BitVector::set_val(int pos,
		   Val3 val)
{
  uniquefy();

  mPtr->set_val(pos, val);
}

// @brief BIN文字列から内容を設定する．
// @param[in] bin_string BIN文字列
// @retval true 適切に設定された．
// @retval false bin_string に不適切な文字が含まれていた．
//
// - bin_string がベクタ長より短い時には残りはXで初期化される．
// - bin_string がベクタ長より長い時には余りは切り捨てられる．
// - 有効な文字は '0', '1', 'x', 'X'
inline
bool
BitVector::set_from_bin(const string& bin_string)
{
  uniquefy();

  return mPtr->set_from_bin(bin_string);
}

// @brief HEX文字列から内容を設定する．
// @param[in] hex_string HEX 文字列
// @retval true 適切に設定された．
// @retval false hex_string に不適切な文字が含まれていた．
//
// - hex_string が短い時には残りは0で初期化される．
// - hex_string が長い時には余りは捨てられる．
inline
bool
BitVector::set_from_hex(const string& hex_string)
{
  uniquefy();

  return mPtr->set_from_hex(hex_string);
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
//
// - 結果はかならず 0 か 1 になる．(Xは含まれない)
template<class URNG>
inline
void
BitVector::set_from_random(URNG& randgen)
{
  uniquefy();

  mPtr->set_from_random(randgen);
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
template<class URNG>
inline
void
BitVector::fix_x_from_random(URNG& randgen)
{
  uniquefy();

  mPtr->fix_x_from_random(randgen);
}

// @brief 多重参照の場合に複製して単一参照にする．
//
// 内容を書き換える前に呼ばれる．
inline
void
BitVector::uniquefy()
{
  if ( !mPtr.unique() ) {
    // 内容を変更するので複製する．
    mPtr = std::shared_ptr<BitVectorRep>(BitVectorRep::new_vector(*mPtr));
  }
}

END_NAMESPACE_DRUID

#endif // BITVECTOR_H
