#ifndef DIFFBITSARRAY_H
#define DIFFBITSARRAY_H

/// @file DiffBitsArray.h
/// @brief DiffBitsArray のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/PackedVal.h"
#include "fsim/DiffBits.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiffBitsArray DiffBitsArray.h "DiffBitsArray.h"
/// @brief DiffBits の配列
///
/// 実際には非ゼロの出力要素ごとに PackedVal を持つ．
//////////////////////////////////////////////////////////////////////
class DiffBitsArray
{
  struct Cell {
    SizeType output_id;
    PackedVal bits;

    bool
    operator==(
      const Cell& right
    ) const
    {
      return output_id == right.output_id && bits == right.bits;
    }
  };


public:

  /// @brief コンストラクタ
  DiffBitsArray() = default;

  /// @brief デストラクタ
  ~DiffBitsArray() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 非ゼロの要素数を返す．
  SizeType
  elem_num() const
  {
    return mBody.size();
  }

  /// @brief 非ゼロの出力番号を返す．
  SizeType
  output(
    SizeType index ///< [in] 位置番号( 0 <= index < elem_num() )
  ) const
  {
    _check_index(index);
    return mBody[index].output_id;
  }

  /// @brief 故障伝搬ビットパタンを返す．
  PackedVal
  dbits(
    SizeType index ///< [in] 位置番号( 0 <= index < elem_num() )
  ) const
  {
    _check_index(index);
    return mBody[index].bits;
  }

  /// @brief 全ての出力のビットパタンの OR を返す．
  PackedVal
  dbits_union() const
  {
    return mUnion;
  }

  /// @brief ビットスライスを得る．
  DiffBits
  get_slice(
    SizeType pos     ///< [in] ビット位置
  ) const
  {
    DiffBits dbits;
    PackedVal mask = 1UL << pos;
    for ( auto& cell: mBody ) {
      if ( (cell.bits & mask) != PV_ALL0 ) {
	dbits.add_output(cell.output_id);
      }
    }
    dbits.sort();
    return dbits;
  }

  /// @brief ビットマスクを行う．
  DiffBitsArray
  masking(
    PackedVal mask
  ) const
  {
    DiffBitsArray ans;
    for ( auto& cell: mBody ) {
      auto bits = cell.bits & mask;
      if ( bits != PV_ALL0 ) {
	ans.mBody.push_back({cell.output_id, bits});
      }
    }
    ans.mUnion = mUnion & mask;
    return ans;
  }

  /// @brief 内容をクリアする．
  void
  clear()
  {
    mBody.clear();
    mUnion = 0UL;
  }

  /// @brief 出力のビットパタンを追加する．
  void
  add_output(
    SizeType output, ///< [in] 出力番号
    PackedVal dbits  ///< [in] 故障伝搬ビットパタン
  )
  {
    mBody.push_back({output, dbits});
    mUnion |= dbits;
  }

  /// @brief 1ビット分のパタンを追加する．
  void
  add_pat(
    const DiffBits& dbits, ///< [in] ビットパタン
    SizeType pos           ///< [in] ビット位置
  )
  {
    PackedVal mask = 1UL << pos;
    std::unordered_map<SizeType, SizeType> pos_map;
    for ( SizeType i = 0; i < mBody.size(); ++ i ) {
      pos_map.emplace(mBody[i].output_id, i);
    }
    for ( SizeType i = 0; i < dbits.elem_num(); ++ i ) {
      auto oid = dbits.output(i);
      if ( pos_map.count(oid) ) {
	SizeType j = pos_map.at(oid);
	mBody[j].bits |= mask;
      }
      else {
	mBody.push_back({oid, mask});
      }
    }
    mUnion |= mask;
  }

  /// @brief 内容をソートする．
  void
  sort()
  {
    std::sort(mBody.begin(), mBody.end(),
	      [&](
		const Cell& a,
		const Cell& b
	      )
	      {
		return a.output_id < b.output_id;
	      }
	      );
  }

  /// @brief ソートした内容を返す．
  DiffBitsArray
  sorted() const
  {
    DiffBitsArray ans{*this};
    ans.sort();
    return ans;
  }

  /// @brief 等価比較演算
  bool
  operator==(
    const DiffBitsArray& right
  ) const
  {
    return mBody == right.mBody;
  }

  /// @brief 非等価比較演算
  bool
  operator!=(
    const DiffBitsArray& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s
  ) const
  {
    for ( auto& cell: mBody ) {
      auto pos = cell.output_id;
      auto bits = cell.bits;
      s << " " << pos << ": "
	<< std::hex << std::setw(16) << bits << std::dec;
    }
  }

  /// @brief ハッシュ関数
  SizeType
  hash() const
  {
    SizeType ans = 0;
    for ( auto& cell: mBody ) {
      auto pos = cell.output_id;
      auto bits = cell.bits;
      ans = (ans * 1021) + pos;
      ans = (ans * 2017) + bits;
    }
    return ans;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief インデックスの範囲チェック
  void
  _check_index(
    SizeType index
  ) const
  {
    if ( index >= mBody.size() ) {
      throw std::out_of_range{"index is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 非ゼロ要素の出力番号とビットパタンのペアのリスト
  std::vector<Cell> mBody;

  // mBody の各要素のビットワイズOR
  PackedVal mUnion{0UL};

};

/// @brief ストリーム出力演算子
inline
std::ostream&
operator<<(
  std::ostream& s,
  const DiffBitsArray& dbits
)
{
  dbits.print(s);
  return s;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

// DiffBitsArray をキーとしたハッシュ関数クラス
template <>
struct hash<DRUID_NAMESPACE::DiffBitsArray>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::DiffBitsArray& dbits
  ) const
  {
    return dbits.hash();
  }
};

END_NAMESPACE_STD

#endif // DIFFBITSARRAY_H
