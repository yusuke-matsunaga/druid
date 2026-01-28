#ifndef BITVECTOR_H
#define BITVECTOR_H

/// @file BitVector.h
/// @brief BitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Val3.h"
#include "PackedVal.h"
#include <random>


BEGIN_NAMESPACE_DRUID

class BitVectorRep;

//////////////////////////////////////////////////////////////////////
/// @class BitVector BitVector.h "BitVector.h"
/// @brief 入力用のビットベクタを表すクラス
/// @ingroup TypesGroup
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
  );

  /// @brief コピーコンストラクタ
  ///
  /// '浅い'コピーとなる．
  BitVector(
    const BitVector& src  ///< [in] コピー元のソース
  );

  /// @brief コピー代入演算子
  ///
  /// '浅い'コピーとなる．
  BitVector&
  operator=(
    const BitVector& src  ///< [in] コピー元のソース
  );

  /// @brief 2進文字列からオブジェクトを作る．
  ///
  /// - ベクタ長は文字列の長さから得る．
  /// - 文字列は '0', '1', 'X', 'x' で構成される．
  /// - 最初の文字が0ビット目となる．
  /// - bin_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_bin(
    const std::string& bin_str  ///< [in] 2進文字列
  );

  /// @brief HEX文字列からオブジェクトを作る．
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - hex_str が不適切な場合には長さ0のベクタを返す．
  static
  BitVector
  from_hex(
    SizeType len,              ///< [in] ベクタ長
    const std::string& hex_str ///< [in] HEX文字列
  );

  /// @brief デストラクタ
  ~BitVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  SizeType
  len() const;

  /// @brief ブロック長を返す．
  SizeType
  block_num() const
  {
    return (len() + PV_BITLEN - 1) / PV_BITLEN;
  }

  /// @brief 値を得る．
  Val3
  val(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < len() )
  ) const;

  /// @brief X の個数を得る．
  SizeType
  x_count() const;

  /// @brief マージして代入する．
  BitVector&
  operator&=(
    const BitVector& right
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
  ///
  /// ビット長は変わらないので左端の1ビットは捨てられる．
  void
  lshift(
    Val3 new_val ///< [in] 右端に挿入する値
  );

  /// @brief 1ビット右シフトを行う．
  ///
  /// ビット長は変わらないので右端の1ビットは捨てられる．
  void
  rshift(
    Val3 new_val ///< [in] 左端に挿入する値
  );

  /// @brief 値を設定する．
  void
  set_val(
    SizeType pos, ///< [in] 位置番号 ( 0 <= pos < len() )
    Val3 val      ///< [in] 値
  );

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
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  /// - 有効な文字は '0'〜'9', 'a'〜'f', 'A'〜'F'
  /// - この形式は X を扱えない．
  bool
  set_from_hex(
    const std::string& hex_string  ///< [in] HEX 文字列
  );

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

    std::uniform_int_distribution<PackedVal> rd;
    auto nb = block_num();
    for ( SizeType i = 0; i < nb; ++ i ) {
      auto v = rd(randgen);
      set_block(i, ~v, v);
    }
  }

  /// @brief X の部分を乱数で 0/1 に設定する．
  template<class URNG>
  void
  fix_x_from_random(
    URNG& randgen  ///< [in] 乱数生成器
  )
  {
    uniquefy();

    std::uniform_int_distribution<PackedVal> rd;
    auto nb = block_num();
    for ( SizeType i = 0; i < nb; ++ i ) {
      auto v = rd(randgen);
      fix_block(i, ~v, v);
    }
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 2項演算
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立関係の比較を行う．
  /// @return left と right が両立する時 true を返す．
  bool
  operator&&(
    const BitVector& right  ///< [in] オペランド2
  ) const;

  /// @brief 等価関係の比較を行なう．
  /// @return left と right が等しいとき true を返す．
  bool
  operator==(
    const BitVector& right ///< [in] オペランド2
  ) const;

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
  ) const;

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
  ) const;

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


public:
  //////////////////////////////////////////////////////////////////////
  // その他
  //////////////////////////////////////////////////////////////////////

  /// @brief ハッシュ値を計算する．
  SizeType
  hash() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 多重参照の場合に複製して単一参照にする．
  ///
  /// 内容を書き換える前に呼ばれる．
  void
  uniquefy();

  /// @brief 値をセットする．
  void
  set_block(
    SizeType pos, ///< [in] 位置 ( 0 <= pos < block_num() )
    PackedVal v0, ///< [in] ブロック0の値
    PackedVal v1  ///< [in] ブロック1の値
  );

  /// @brief X の部分に値をセットする．
  void
  fix_block(
    SizeType pos, ///< [in] 位置 ( 0 <= pos < block_num() )
    PackedVal v0, ///< [in] ブロック0の値
    PackedVal v1  ///< [in] ブロック1の値
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::shared_ptr<BitVectorRep> mPtr;

};

END_NAMESPACE_DRUID

#endif // BITVECTOR_H
