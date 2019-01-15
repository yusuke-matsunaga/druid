#ifndef TESTVECTOR_H
#define TESTVECTOR_H

/// @file TestVector.h
/// @brief TestVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"
#include "BitVector.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestVector TestVector.h "TestVector.h"
/// @brief テストベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
///
/// スキャン方式の縮退故障用ベクタと
/// ブロードサイド方式の遷移故障用ベクタの共用となる．
/// 縮退故障用の時は
/// - 外部入力(InputVector)
/// - DFF(DffVector)
/// の値を持つ．
///
/// 遷移故障用の時は
/// - 外部入力[1時刻目](InputVector)
/// - DFF[1時刻目](DffVector)
/// - 外部入力[2時刻目](InputVector)
/// の値を持つ．
///
/// @sa InputVector, DffVector
//////////////////////////////////////////////////////////////////////
class TestVector
{
public:

  /// @brief 空のコンストラクタ
  TestVector();

  /// @brief コンストラクタ(組み合わせ回路用)
  /// @param[in] input_num 入力数
  ///
  /// 故障タイプは StuckAt
  TestVector(int input_num);

  /// @brief コンストラクタ(順序回路用)
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  TestVector(int input_num,
	     int dff_num,
	     FaultType fault_type);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  TestVector(const TestVector& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  TestVector&
  operator=(const TestVector& src);

  /// @brief 割当リストからTestVectorを作るクラスメソッド
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  /// @param[in] assign_list 割当リスト
  /// @return assign_list から変換したテストベクタ
  ///
  /// assign_list に外部入力とDFF以外の割当が含まれている場合無視する．
  static
  TestVector
  new_from_assign_list(int input_num,
		       int dff_num,
		       FaultType fault_type,
		       const NodeValList& assign_list);

  /// @brief HEX文字列からTestVectorを作るクラスメソッド
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  /// @param[in] hex_string HEX 文字列
  /// @return 生成したテストベクタ
  ///
  /// 1時刻目の外部入力，１時刻目のDFF，２時刻目の外部入力の順にならんでいると仮定する．<br>
  /// hex_string が短い時には残りはXで初期化される．<br>
  /// hex_string が長い時には余りは捨てられる．<br>
  static
  TestVector
  new_from_hex(int input_num,
	       int dff_num,
	       FaultType fault_type,
	       const string& hex_string);

  /// @brief デストラクタ
  ~TestVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を得る．
  int
  vector_size() const;

  /// @brief 値を得る．
  /// @param[in] pos ビット位置 ( 0 <= pos < vector_size() )
  Val3
  val(int pos) const;

  /// @brief 外部入力数を得る．
  int
  input_num() const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief ２時刻目の外部入力を持つ時 true を返す．
  bool
  has_aux_input() const;

  /// @brief PPI数を得る．
  ///
  /// = input_num() + dff_num()
  int
  ppi_num() const;

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief PPIの値を得る．
  /// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
  Val3
  ppi_val(int pos) const;

  /// @brief 1時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos) と同じ．
  Val3
  input_val(int pos) const;

  /// @brief 1時刻目のDFFの値を得る．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos + input_num()) と同じ．
  Val3
  dff_val(int pos) const;

  /// @brief 2時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効
  Val3
  aux_input_val(int pos) const;

  /// @brief X の個数を得る．
  int
  x_count() const;

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // TestVector の演算
  //////////////////////////////////////////////////////////////////////

  /// @brief マージして代入する．
  TestVector&
  operator&=(const TestVector& right);

  /// @brief マージして代入する．
  void
  merge(const TestVector& right);


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @brief PPIの値を設定する．
  /// @param[in] pos PPIの位置番号 ( 0 <= pos < ppi_num() )
  /// @param[in] val 値
  ///
  /// is_sa_mode() == true の時のみ有効
  void
  set_ppi_val(int pos,
	      Val3 val);

  /// @breif 1時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_input_val(int pos,
		Val3 val);

  /// @breif 1時刻目のDFFの値を設定する．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_dff_val(int pos,
	      Val3 val);

  /// @breif 2時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ意味を持つ．
  void
  set_aux_input_val(int pos,
		    Val3 val);

  /// @brief 乱数パタンを設定する．
  /// @param[in] randgen 乱数生成器
  /// @note 結果はかならず 0 か 1 になる．(Xは含まれない)
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
  operator&&(const TestVector& left,
	     const TestVector& right);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] left, right オペランド
  /// @return left と right が等しいとき true を返す．
  friend
  bool
  operator==(const TestVector& left,
	     const TestVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<(const TestVector& left,
	    const TestVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<=(const TestVector& left,
	     const TestVector& right);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を計算する．
  ///
  /// - fault_type() == FaultType::StuckAt の時は input_num() + dff_num()
  /// - fault_type() == FaultType::TransitionDelay の時は input_num() * 2 + dff_num()
  int
  _calc_vect_len() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  int mInputNum;

  // DFF数
  int mDffNum;

  // 故障のタイプ
  FaultType mFaultType;

  // 本体のビットベクタ
  BitVector mVector;

};

/// @relates TestVector
/// @brief 2つのベクタが両立するとき true を返す．
/// @param[in] tv1, tv2 対象のテストベクタ
///
/// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
bool
operator&&(const TestVector& tv1,
	   const TestVector& tv2);

/// @relates TestVector
/// @brief operator&& の別名
/// @param[in] tv1, tv2 対象のテストベクタ
///
/// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
bool
is_compatible(const TestVector& tv1,
	      const TestVector& tv2);

/// @relates TestVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しいとき true を返す．
bool
operator==(const TestVector& left,
	   const TestVector& right);

/// @relates TestVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しいとき true を返す．
bool
is_equal(const TestVector& left,
	 const TestVector& right);

/// @relates TestVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
bool
operator!=(const TestVector& left,
	   const TestVector& right);

/// @relates TestVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<(const TestVector& left,
	  const TestVector& right);

/// @relates TestVector
/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note false だからといって逆に right が left を含むとは限らない．
bool
operator>(const TestVector& left,
	  const TestVector& right);

/// @relates TestVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<=(const TestVector& left,
	   const TestVector& right);

/// @relates TestVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note こちらは等しい場合も含む．
/// @note false だからといって逆に right が left を含むとは限らない．
bool
operator>=(const TestVector& left,
	   const TestVector& right);

/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
TestVector
operator&(const TestVector& left,
	  const TestVector& right);

/// @brief 複数のテストベクタをマージする．
/// @param[in] tv_list マージするテストベクタのリスト
/// @return マージ結果を返す．
///
/// tv_list の要素が互いにコンフリクトしている時の結果は不定
TestVector
merge(const vector<TestVector>& tv_list);

/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] tv テストベクタ
ostream&
operator<<(ostream& s,
	   const TestVector& tv);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ベクタ長を計算する．
inline
int
TestVector::_calc_vect_len() const
{
  int x = mFaultType == FaultType::StuckAt ? 1 : 2;
  return mInputNum * x + mDffNum;
}

// @brief 空のコンストラクタ
inline
TestVector::TestVector() :
  mInputNum(0),
  mDffNum(0),
  mFaultType(FaultType::StuckAt),
  mVector(_calc_vect_len())
{
}

// @brief コンストラクタ(組み合わせ回路用)
// @param[in] input_num 入力数
//
// 故障タイプは StuckAt
inline
TestVector::TestVector(int input_num) :
  mInputNum(input_num),
  mDffNum(0),
  mFaultType(FaultType::StuckAt),
  mVector(_calc_vect_len())
{
}

// @brief コンストラクタ(順序回路用)
// @param[in] input_num 入力数
// @param[in] dff_numr DFF数
// @param[in] fault_type 故障の種類
inline
TestVector::TestVector(int input_num,
		       int dff_num,
		       FaultType fault_type) :
  mInputNum(input_num),
  mDffNum(dff_num),
  mFaultType(fault_type),
  mVector(_calc_vect_len())
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
inline
TestVector::TestVector(const TestVector& src) :
  mInputNum(src.mInputNum),
  mDffNum(src.mDffNum),
  mFaultType(src.mFaultType),
  mVector(src.mVector)
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
inline
TestVector&
TestVector::operator=(const TestVector& src)
{
  mInputNum = src.mInputNum;
  mDffNum = src.mDffNum;
  mFaultType = mFaultType;
  mVector = src.mVector;

  return *this;
}

// @brief HEX文字列からTestVectorを作るクラスメソッド
// @param[in] input_num 入力数
// @param[in] dff_numr DFF数
// @param[in] fault_type 故障の種類
// @param[in] hex_string HEX 文字列
// @return 生成したテストベクタ
//
// 1時刻目の外部入力，１時刻目のDFF，２時刻目の外部入力の順にならんでいると仮定する．<br>
// hex_string が短い時には残りはXで初期化される．<br>
// hex_string が長い時には余りは捨てられる．<br>
inline
TestVector
TestVector::new_from_hex(int input_num,
			 int dff_num,
			 FaultType fault_type,
			 const string& hex_string)
{
  TestVector tv(input_num, dff_num, fault_type);
  tv.mVector.set_from_hex(hex_string);

  return tv;
}

// @brief デストラクタ
inline
TestVector::~TestVector()
{
}

// @brief ベクタ長を得る．
inline
int
TestVector::vector_size() const
{
  return mVector.len();
}

// @brief 値を得る．
// @param[in] pos ビット位置 ( 0 <= pos < vector_size() )
inline
Val3
TestVector::val(int pos) const
{
  return mVector.val(pos);
}

// @brief 入力数を得る．
inline
int
TestVector::input_num() const
{
  return mInputNum;
}

// @brief DFF数を得る．
inline
int
TestVector::dff_num() const
{
  return mDffNum;
}

// @brief PPI数を得る．
//
// = input_num() + dff_num()
inline
int
TestVector::ppi_num() const
{
  return input_num() + dff_num();
}

// @brief ２時刻目の外部入力を持つ時 true を返す．
inline
bool
TestVector::has_aux_input() const
{
  return fault_type() == FaultType::TransitionDelay;
}

// @brief 故障の種類を返す．
inline
FaultType
TestVector::fault_type() const
{
  return mFaultType;
}

// @brief PPIの値を得る．
// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
inline
Val3
TestVector::ppi_val(int pos) const
{
  return mVector.val(pos);
}

// @brief 1時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::input_val(int pos) const
{
  return mVector.val(pos);
}

// @brief 1時刻目のDFFの値を得る．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
inline
Val3
TestVector::dff_val(int pos) const
{
  return mVector.val(pos + mInputNum);
}

// @brief 2時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::aux_input_val(int pos) const
{
  return mVector.val(pos + ppi_num());
}

// @brief PPIの値を設定する．
// @param[in] pos PPIの位置番号 ( 0 <= pos < ppi_num() )
// @param[in] val 値
//
// is_sa_mode() == true の時のみ有効
inline
void
TestVector::set_ppi_val(int pos,
			Val3 val)
{
  mVector.set_val(pos, val);
}

// @breif 1時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_input_val(int pos,
			  Val3 val)
{
  mVector.set_val(pos, val);
}

// @breif 1時刻目のDFFの値を設定する．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
// @param[in] val 値
inline
void
TestVector::set_dff_val(int pos,
			Val3 val)
{
  mVector.set_val(pos + input_num(), val);
}

// @breif 2時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_aux_input_val(int pos,
			      Val3 val)
{
  mVector.set_val(pos + ppi_num(), val);
}

// @brief X の個数を得る．
inline
int
TestVector::x_count() const
{
  return mVector.x_count();
}

// @brief マージして代入する．
inline
void
TestVector::merge(const TestVector& right)
{
  operator&=(right);
}

// @brief 2つのベクタが両立しないとき true を返す．
inline
bool
operator&&(const TestVector& tv1,
	   const TestVector& tv2)
{
  return tv1.mVector && tv2.mVector;
}

// @relates TestVector
// @brief operator&& の別名
// @param[in] tv1, tv2 対象のテストベクタ
//
// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
inline
bool
is_compatible(const TestVector& tv1,
	      const TestVector& tv2)
{
  return tv1 && tv2;
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しいとき true を返す．
inline
bool
operator==(const TestVector& left,
	   const TestVector& right)
{
  return left.mVector == right.mVector;
}

// @relates TestVector
// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しいとき true を返す．
inline
bool
is_equal(const TestVector& left,
	 const TestVector& right)
{
  return left == right;
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const TestVector& left,
	   const TestVector& right)
{
  return !operator==(left, right);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator<(const TestVector& left,
	  const TestVector& right)
{
  return left.mVector < right.mVector;
}

// @brief 包含関係の比較を行なう．
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(const TestVector& left,
	  const TestVector& right)
{
  return operator<(right, left);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - こちらは等しい場合も含む．
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator<=(const TestVector& left,
	   const TestVector& right)
{
  return left.mVector <= right.mVector;
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
// @note こちらは等しい場合も含む．
// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>=(const TestVector& left,
	   const TestVector& right)
{
  return operator<=(right, left);
}

// @brief すべて未定(X) で初期化する．
inline
void
TestVector::init()
{
  mVector.init();
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
template<class URNG>
inline
void
TestVector::set_from_random(URNG& randgen)
{
  mVector.set_from_random(randgen);
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
template<class URNG>
inline
void
TestVector::fix_x_from_random(URNG& randgen)
{
  mVector.fix_x_from_random(randgen);
}

// @brief 内容を BIN 形式で表す．
inline
string
TestVector::bin_str() const
{
  return mVector.bin_str();
}

// @brief 内容を HEX 形式で出力する．
inline
string
TestVector::hex_str() const
{
  return mVector.hex_str();
}

// @brief マージする．
// @param[in] left, right オペランド
// @return マージ結果を返す．
//
// left と right がコンフリクトしている時の結果は不定
inline
TestVector
operator&(const TestVector& left,
	  const TestVector& right)
{
  return TestVector(left).operator&=(right);
}

// @brief マージして代入する．
inline
TestVector&
TestVector::operator&=(const TestVector& right)
{
  mVector &= right.mVector;

  return *this;
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const TestVector& tv)
{
  return s << tv.bin_str();
}

END_NAMESPACE_DRUID

#endif // TESTVECTOR_H
