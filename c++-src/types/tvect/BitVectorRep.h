#ifndef BITVECTORREP_H
#define BITVECTORREP_H

/// @file BitVectorRep.h
/// @brief BitVectorRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/Val3.h"
#include "types/PackedVal.h"
#include <random>


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BitVectorRep BitVectorRep.h "BitVectorRep.h"
/// @brief ビットベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
///
/// mPat[i * 2 + 0], mPat[i * 2 + 1] の２つのワードでそれぞれ0のビット
/// と1のビットを表す．
/// X の場合は両方のビットに1を立てる．
///
/// 要するに mPat[i * 2 + 0], mPat[i * 2 + 1] のビットが
/// - (0, 0): 未使用
/// - (1, 0): Val3::_0
/// - (0, 1): Val3::_1
/// - (1, 1): Val3::_X
/// を表している．
///
/// 0 番目のビットは 0 ビットめに対応する．
//////////////////////////////////////////////////////////////////////
class BitVectorRep
{
public:
  //////////////////////////////////////////////////////////////////////
  // オブジェクトを作るクラスメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を指定してオブジェクトを作る．
  ///
  /// 内容は X に初期化される．
  static
  std::shared_ptr<BitVectorRep>
  new_vector(
    SizeType len  ///< [in] ベクタ長
  );

  /// @brief 内容をコピーする．
  static
  std::shared_ptr<BitVectorRep>
  new_vector(
    const BitVectorRep& src  ///< [in] コピー元のオブジェクト
  );


public:

  /// @brief デストラクタ
  ~BitVectorRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  SizeType
  len() const
  {
    return mLength;
  }

  /// @brief ブロック長を返す．
  SizeType
  block_num() const
  {
    return block_num(len());
  }

  /// @brief 値を得る．
  Val3
  val(
    SizeType index  ///< [in] 位置番号 ( 0 <= index < len() )
  ) const
  {
    _check_index(index);

    auto shift = shift_num(index);
    auto block0 = block_idx(index);
    auto block1 = block0 + 1;
    int v0 = (mPat[block0] >> shift) & 1UL;
    int v1 = (mPat[block1] >> shift) & 1UL;
    int tmp = v0 + v0 + v1;
    return static_cast<Val3>(tmp ^ 3);
  }

  /// @brief X の個数を得る．
  SizeType
  x_count() const;

  /// @brief 2つのビットベクタの等価比較を行う．
  /// @return 2つのビットベクタが等しい時 true を返す．
  static
  bool
  is_eq(
    const BitVectorRep& bv1, ///< [in] オペランド1
    const BitVectorRep& bv2  ///< [in] オペランド2
  );

  /// @brief 2つのビットベクタの包含関係を調べる．
  /// @return bv1 が真に bv2 に含まれる時 true を返す．
  static
  bool
  is_lt(
    const BitVectorRep& bv1, ///< [in] オペランド1
    const BitVectorRep& bv2  ///< [in] オペランド2
  );

  /// @brief 2つのビットベクタの包含関係を調べる．
  /// @return bv1 が bv2 に含まれる時 true を返す．
  ///
  /// こちらは bv1 と bv2 が等しい場合も true を返す．
  static
  bool
  is_le(
    const BitVectorRep& bv1, ///< [in] オペランド1
    const BitVectorRep& bv2  ///< [in] オペランド2
  );

  /// @brief 2つのベクタが両立している時 true を返す．
  ///
  /// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
  static
  bool
  is_compat(
    const BitVectorRep& bv1, ///< [in] オペランド1
    const BitVectorRep& bv2  ///< [in] オペランド2
  );

  /// @brief 内容を BIN 形式で表す．
  std::string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  ///
  /// X を含む場合の出力は不定
  std::string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @brief 1ビット左シフトを行う．
  void
  lshift(
    Val3 new_val ///< [in] 右端に挿入する値
  );

  /// @brief 1ビット右シフトを行う．
  void
  rshift(
    Val3 new_val ///< [in] 左端に挿入する値
  );

  /// @brief 値を設定する．
  void
  set_val(
    SizeType index, ///< [in] 位置番号 ( 0 <= index < len() )
    Val3 val        ///< [in] 値
  )
  {
    _check_index(index);

    auto shift = shift_num(index);
    auto block0 = block_idx(index);
    auto block1 = block0 + 1;
    PackedVal mask = 1UL << shift;
    switch ( val ) {
    case Val3::_0:
      mPat[block0] |=  mask;
      mPat[block1] &= ~mask;
      break;
    case Val3::_1:
      mPat[block0] &= ~mask;
      mPat[block1] |=  mask;
      break;
    case Val3::_X:
      mPat[block0] |=  mask;
      mPat[block1] |=  mask;
      break;
    }
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
    const std::string& bin_string  ///< [in] BIN文字列
  );

  /// @brief HEX文字列から内容を設定する．
  /// @retval true 適切に設定された．
  /// @retval false hex_string に不適切な文字が含まれていた．
  ///
  /// - hex_string が短い時には残りはXで初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - この形式は X を扱えない．
  bool
  set_from_hex(
    const std::string& hex_string  ///< [in] HEX 文字列
  );

  /// @brief 値をセットする．
  void
  set_block(
    SizeType pos, ///< [in] 位置 ( 0 <= pos < block_num() )
    PackedVal v0, ///< [in] ブロック0の値
    PackedVal v1  ///< [in] ブロック1の値
  )
  {
    auto i0 = pos * 2 + 0;
    auto i1 = i0 + 1;
    if ( i1 == block_num(len()) - 1 ) {
      auto mask = get_mask();
      mPat[i0] = v0 & mask;
      mPat[i1] = v1 & mask;
    }
    else {
      mPat[i0] = v0;
      mPat[i1] = v1;
    }
  }

  /// @brief X の部分に値をセットする．
  void
  fix_block(
    SizeType pos, ///< [in] 位置 ( 0 <= pos < block_num() )
    PackedVal v0, ///< [in] ブロック0の値
    PackedVal v1  ///< [in] ブロック1の値
  )
  {
    auto i0 = pos * 2 + 0;
    auto i1 = i0 + 1;
    auto xmask = mPat[i0] & mPat[i1];
    if ( i1 == block_num(len()) - 1 ) {
      auto mask = get_mask();
      xmask &= mask;
    }
    mPat[i0] &= ~(~v0 & xmask);
    mPat[i1] &= ~(~v1 & xmask);
  }

  /// @brief ビットベクタをマージする．
  ///
  /// X 以外で相異なるビットがあったら false を返す．
  bool
  merge(
    const BitVectorRep& src ///< [in] オペランド
  );


public:
  //////////////////////////////////////////////////////////////////////
  // その他
  //////////////////////////////////////////////////////////////////////

  /// @brief ハッシュ値を求める．
  SizeType
  hash() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ブロック数を返す．
  static
  SizeType
  block_num(
    SizeType length  ///< [in] ベクタ長
  )
  {
    return ((length + PV_BITLEN - 1) / PV_BITLEN) * 2;
  }

  /// @brief HEX文字列の長さを返す．
  static
  SizeType
  hex_length(
    SizeType length  ///< [in] ベクタ長
  )
  {
    return (length + 3) / 4;
  }

  // 位置からブロック番号を得る．
  static
  SizeType
  block_idx(
    SizeType pos  ///< [in] 位置番号
  )
  {
    return (pos / PV_BITLEN) * 2;
  }

  // 位置からシフト量を得る．
  static
  SizeType
  shift_num(
    SizeType pos  ///< [in] 位置番号
  )
  {
    return pos % PV_BITLEN;
  }

  // @brief 末尾ブロックのマスクを求める．
  PackedVal
  get_mask() const
  {
    SizeType k = len() % PV_BITLEN;
    PackedVal mask;
    if ( k == 0 ) {
      mask = PV_ALL1;
    }
    else {
      mask = (1ULL << k) - 1;
    }
    return mask;
  }

  /// @brief インデックスの範囲チェック
  void
  _check_index(
    SizeType index
  ) const
  {
    if ( index >= len() ) {
      abort();
      throw std::out_of_range{"index is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタは外部からは使わせない．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  BitVectorRep(
    SizeType length  ///< [in] ベクタ長
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ベクタ長
  SizeType mLength;

  // ベクタ本体の配列(のダミー)
  PackedVal mPat[0];


private:
  //////////////////////////////////////////////////////////////////////
  // このクラスに固有の定数
  //////////////////////////////////////////////////////////////////////

  // 1ワードあたりのHEX文字数
  static
  const SizeType HPW = PV_BITLEN / 4;

};

/// @relates BitVectorRep
/// @brief 内容を出力する．
inline
std::ostream&
operator<<(
  std::ostream& s,       ///< [in] 出力先のストリーム
  const BitVectorRep& bv ///< [in] ビットベクタ
)
{
  return s << bv.bin_str();
}

/// @brief 内容を出力する．
inline
std::ostream&
operator<<(
  std::ostream& s,        ///< [in] 出力先のストリーム
  const BitVectorRep* bvp ///< [in] ビットベクタへのポインタ
)
{
  return s << bvp->bin_str();
}

END_NAMESPACE_DRUID

#endif // BITVECTORREP_H
