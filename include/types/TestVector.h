#ifndef TESTVECTOR_H
#define TESTVECTOR_H

/// @file TestVector.h
/// @brief TestVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "AssignList.h"
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
  TestVector(
  ) : mInputNum{0},
      mDffNum{0},
      mVector{_calc_vect_len()}
  {
  }

  /// @brief コンストラクタ(組み合わせ回路用)
  TestVector(
    SizeType input_num  ///< [in] 入力数
  ) : mInputNum{input_num},
      mDffNum{0},
      mVector{_calc_vect_len()}
  {
  }

  /// @brief コンストラクタ(組み合わせ回路用)
  TestVector(
    SizeType input_num,  ///< [in] 入力数
    const BitVector& src ///< [in] 内容を表すビットベクタ
  ) : mInputNum{input_num},
      mDffNum{0},
      mVector{src}
  {
  }

  /// @brief コンストラクタ(順序回路用)
  TestVector(
    SizeType input_num,  ///< [in] 入力数
    SizeType dff_num,	 ///< [in] DFF数
    bool has_prev_state  ///< [in] 1時刻前の回路を持つ時 true
  ) : mInputNum{input_num},
      mDffNum{(dff_num << 1) | static_cast<SizeType>(has_prev_state)},
      mVector{_calc_vect_len()}
  {
  }

  /// @brief コンストラクタ(順序回路用)
  TestVector(
    SizeType input_num,  ///< [in] 入力数
    SizeType dff_num,	 ///< [in] DFF数
    bool has_prev_state, ///< [in] 1時刻前の回路を持つ時 true
    const BitVector& src ///< [in] 内容を表すビットベクタ
  ) : mInputNum{input_num},
      mDffNum{(dff_num << 1) | static_cast<SizeType>(has_prev_state)},
      mVector{src}
  {
  }

  /// @brief ネットワークから入力数などの諸元を取ってくるコンストラクタ
  explicit
  TestVector(
    const TpgNetwork& network
  ) : TestVector{
      network.input_num(),
      network.dff_num(),
      network.fault_type() == FaultType::TransitionDelay
    }
  {
  }

  /// @brief ネットワークから入力数などの諸元を取ってくるコンストラクタ
  explicit
  TestVector(
    const TpgNetwork& network,
    const AssignList& pi_assign_list
  ) : TestVector{network}
  {
    set_from_assign_list(pi_assign_list);
  }

  /// @brief コピーコンストラクタ
  TestVector(
    const TestVector& src///< [in] コピー元のソース
  ) : mInputNum{src.mInputNum},
      mDffNum{src.mDffNum},
      mVector{src.mVector}
  {
  }

  /// @brief コピー代入演算子
  TestVector&
  operator=(
    const TestVector& src ///< [in] コピー元のソース
  )
  {
    mInputNum = src.mInputNum;
    mDffNum = src.mDffNum;
    mVector = src.mVector;

    return *this;
  }

  /// @brief 2進文字列からオブジェクトを作る．
  ///
  /// - 有効な文字は '0', '1', 'x', 'X'
  /// - bin_str が不適切な場合には長さ0のベクタを返す．
  static
  TestVector
  from_bin(
    SizeType input_num,        ///< [in] 入力数
    SizeType dff_num,          ///< [in] DFF数
    bool has_prev_state,       ///< [in] 1時刻前の値を持つ時 true
    const std::string& bin_str ///< [in] 元となる2進文字列
  );

  /// @brief HEX文字列からオブジェクトを作る．
  ///
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - hex_str が不適切な場合には長さ0のベクタを返す．
  static
  TestVector
  from_hex(
    SizeType input_num,        ///< [in] 入力数
    SizeType dff_num,          ///< [in] DFF数
    bool has_prev_state,       ///< [in] 1時刻前の値を持つ時 true
    const std::string& hex_str ///< [in] 元となるHEX文字列
  );

  /// @brief デストラクタ
  ~TestVector() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を得る．
  SizeType
  vector_size() const
  {
    return mVector.len();
  }

  /// @brief 値を得る．
  Val3
  val(
    SizeType pos ///< [in] ビット位置 ( 0 <= pos < vector_size() )
  ) const
  {
    return mVector.val(pos);
  }

  /// @brief 外部入力数を得る．
  SizeType
  input_num() const
  {
    return mInputNum;
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const
  {
    return mDffNum >> 1;
  }

  /// @brief PPI数を得る．
  ///
  /// = input_num() + dff_num()
  SizeType
  ppi_num() const
  {
    return input_num() + dff_num();
  }

  /// @brief ２時刻目の外部入力を持つ時 true を返す．
  bool
  has_aux_input() const
  {
    return static_cast<bool>(mDffNum & 1);
  }

  /// @brief PPIの値を得る．
  Val3
  ppi_val(
    SizeType pos ///< [in] PPI の位置番号 ( 0 <= pos < ppi_num() )
  ) const
  {
    return mVector.val(pos);
  }

  /// @brief 1時刻目の外部入力の値を得る．
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos) と同じ．
  Val3
  input_val(
    SizeType pos ///< [in] 入力の位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    return mVector.val(pos);
  }

  /// @brief 1時刻目のDFFの値を得る．
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos + input_num()) と同じ．
  Val3
  dff_val(
    SizeType pos ///< [in] DFFの位置番号 ( 0 <= pos < dff_num() )
  ) const
  {
    return mVector.val(pos + mInputNum);
  }

  /// @brief 2時刻目の外部入力の値を得る．
  ///
  /// is_td_mode() == true の時のみ有効
  Val3
  aux_input_val(
    SizeType pos ///< [in] 入力の位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    return mVector.val(pos + ppi_num());
  }

  /// @brief X の個数を得る．
  SizeType
  x_count() const
  {
    return mVector.x_count();
  }

  /// @brief 内容を BIN 形式で表す．
  std::string
  bin_str() const
  {
    return mVector.bin_str();
  }

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  std::string
  hex_str() const
  {
    return mVector.hex_str();
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init()
  {
    mVector.init();
  }

  /// @brief 割当リストから値を設定する．
  ///
  /// assign_list に外部入力とDFF以外の割当が含まれている場合無視する．
  void
  set_from_assign_list(
    const AssignList& assign_list ///< [in] 割当リスト
  );

  /// @brief PPIの値を設定する．
  ///
  /// is_sa_mode() == true の時のみ有効
  void
  set_ppi_val(
    SizeType pos, ///< [in] PPIの位置番号 ( 0 <= pos < ppi_num() )
    Val3 val	  ///< [in] 値
  )
  {
    mVector.set_val(pos, val);
  }

  /// @breif 1時刻目の外部入力の値を設定する．
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_input_val(
    SizeType pos, ///< [in] 入力の位置番号 ( 0 <= pos < input_num() )
    Val3 val	  ///< [in] 値
  )
  {
    mVector.set_val(pos, val);
  }

  /// @breif 1時刻目のDFFの値を設定する．
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_dff_val(
    SizeType pos, ///< [in] DFFの位置番号 ( 0 <= pos < dff_num() )
    Val3 val	  ///< [in] 値
  )
  {
    mVector.set_val(pos + input_num(), val);
  }

  /// @breif 2時刻目の外部入力の値を設定する．
  ///
  /// is_td_mode() == true の時のみ意味を持つ．
  void
  set_aux_input_val(
    SizeType pos, ///< [in] 入力の位置番号 ( 0 <= pos < input_num() )
    Val3 val	  ///< [in] 値
  )
  {
    mVector.set_val(pos + ppi_num(), val);
  }

  /// @brief 乱数パタンを設定する．
  ///
  /// 結果はかならず 0 か 1 になる．(Xは含まれない)
  template<class URNG>
  void
  set_from_random(
    URNG& randgen ///< [in] randgen 乱数生成器
  )
  {
    mVector.set_from_random(randgen);
  }

  /// @brief X の部分を乱数で 0/1 に設定する．

  template<class URNG>
  void
  fix_x_from_random(
    URNG& randgen ///< [in] 乱数生成器
  )
  {
    mVector.fix_x_from_random(randgen);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 二項演算
  //////////////////////////////////////////////////////////////////////

  /// @brief マージする．
  /// @return マージ結果を返す．
  ///
  /// right がコンフリクトしている時の結果は不定
  TestVector
  operator&(
    const TestVector& right ///< [in] オペランド2
  ) const
  {
    return TestVector(*this).operator&=(right);
  }

  /// @brief マージする(operator&の別名)．
  TestVector
  merge(
    const TestVector& right ///< [in] オペランド
  ) const
  {
    return operator&(right);
  }

  /// @brief マージして代入する．
  TestVector&
  operator&=(
    const TestVector& right ///< [in] オペランド
  )
  {
    mVector &= right.mVector;

    return *this;
  }

  /// @brief マージして代入する(operator&=の別名)．
  TestVector&
  merge_int(
    const TestVector& right ///< [in] オペランド
  )
  {
    return operator&=(right);
  }

  /// @brief 両立関係の比較を行う．
  /// @return right が両立する時 true を返す．
  bool
  operator&&(
    const TestVector& right ///< [in] オペランド2
  ) const
  {
    return mVector && right.mVector;
  }

  /// @brief 等価関係の比較を行なう．
  /// @return right が等しいとき true を返す．
  bool
  operator==(
    const TestVector& right ///< [in] オペランド2
  ) const
  {
    return mVector == right.mVector;
  }

  /// @brief 包含関係の比較を行なう
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - false だからといって逆に left が right を含むとは限らない．
  bool
  operator<(
    const TestVector& right ///< [in] オペランド2
  ) const
  {
    return mVector < right.mVector;
  }

  /// @brief 包含関係の比較を行なう
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に left が right を含むとは限らない．
  bool
  operator<=(
    const TestVector& right ///< [in] オペランド2
  ) const
  {
    return mVector <= right.mVector;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を計算する．
  ///
  /// - fault_type() == FaultType::StuckAt の時は input_num() + dff_num()
  /// - fault_type() == FaultType::TransitionDelay の時は input_num() * 2 + dff_num()
  SizeType
  _calc_vect_len() const
  {
    SizeType x = (mDffNum & 1) + 1;
    return mInputNum * x + dff_num();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  SizeType mInputNum;

  // DFF数(最下位ビットは1時刻前の値を持つかどうかのフラグ)
  SizeType mDffNum;

  // 本体のビットベクタ
  BitVector mVector;

};

/// @relates TestVector
/// @brief operator&& の別名
///
/// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
inline
bool
is_compatible(
  const TestVector& tv1, ///< [in] オペランド1
  const TestVector& tv2	 ///< [in] オペランド2
)
{
  return tv1 && tv2;
}

/// @relates TestVector
/// @brief 等価関係の比較を行なう．
/// @return left と right が等しいとき true を返す．
inline
bool
is_equal(
  const TestVector& left, ///< [in] オペランド1
  const TestVector& right ///< [in] オペランド2
)
{
  return left == right;
}

/// @relates TestVector
/// @brief 等価関係の比較を行なう．
/// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(
  const TestVector& left, ///< [in] オペランド1
  const TestVector& right ///< [in] オペランド2
)
{
  return !left.operator==(right);
}

/// @relates TestVector
/// @brief 包含関係の比較を行なう．
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(
  const TestVector& left, ///< [in] オペランド1
  const TestVector& right ///< [in] オペランド2
)
{
  return right.operator<(left);
}

/// @relates TestVector
/// @brief 包含関係の比較を行なう
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// こちらは等しい場合も含む．
/// false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>=(
  const TestVector& left, ///< [in] オペランド1
  const TestVector& right ///< [in] オペランド2
)
{
  return right.operator<=(left);
}

/// @brief 複数のテストベクタをマージする．
/// @return マージ結果を返す．
///
/// tv_list の要素が互いにコンフリクトしている時の結果は不定
TestVector
merge(
  const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
);

/// @brief 内容を出力する．
/// @return s を返す．
inline
std::ostream&
operator<<(
  std::ostream& s,     ///< [in] 出力先のストリーム
  const TestVector& tv ///< [in] テストベクタ
)
{
  return s << tv.bin_str();
}

END_NAMESPACE_DRUID

#endif // TESTVECTOR_H
