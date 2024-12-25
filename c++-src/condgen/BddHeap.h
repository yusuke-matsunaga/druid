#ifndef BDDHEAP_H
#define BDDHEAP_H

/// @file BddHeap.h
/// @brief BddHeap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BddHeap BddHeap.h "BddHeap.h"
/// @brief Bdd をサイズの昇順に並べるためのヒープ木
//////////////////////////////////////////////////////////////////////
class BddHeap
{
private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct Cell
  {
    Bdd bdd;
    SizeType size{0};

    /// @brief からのコンストラクタ
    Cell() = default;

    /// @brief Bdd を指定したコンストラクタ
    Cell(
      const Bdd& bdd1
    ) : bdd{bdd1},
	size{bdd.size()}
    {
    }

    /// @brief 等価比較演算子
    bool
    operator==(
      const Cell& right
    ) const
    {
      return bdd == right.bdd;
    }

    /// @brief 非等価比較演算子
    bool
    operator!=(
      const Cell& right
    ) const
    {
      return !operator==(right);
    }

    /// @brief 小なり比較演算子
    bool
    operator<(
      const Cell& right
    ) const
    {
      return size < right.size;
    }

    /// @brief 大なり比較演算子
    bool
    operator>(
      const Cell& right
    ) const
    {
      return right.operator<(*this);
    }

    /// @brief 小なりイコール比較演算子
    bool
    operator<=(
      const Cell& right
    ) const
    {
      return !right.operator<(*this);
    }

    /// @brief 大なりイコール比較演算子
    bool
    operator>=(
      const Cell& right
    ) const
    {
      return !operator<(right);
    }

  };


public:

  /// @brief コンストラクタ
  BddHeap() = default;

  /// @brief デストラクタ
  ~BddHeap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mArray.size();
  }

  /// @brief BDDを追加する．
  void
  put(
    const Bdd& bdd
  )
  {
    auto pos = mArray.size();
    mArray.push_back(Cell{bdd});
    move_up(pos);
  }

  /// @brief 最小要素を取り出す．
  Bdd
  get_min()
  {
    auto bdd = mArray.front().bdd;
    mArray[0] = mArray.back();
    mArray.pop_back();
    if ( !mArray.empty() ) {
      move_down(0);
    }
    return bdd;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 上に移動させる．
  void
  move_up(
    SizeType cur_pos ///< [in] 現在の位置
  )
  {
    while ( cur_pos > 0 ) {
      auto cur_cell = mArray[cur_pos];
      auto parent_pos = (cur_pos + 1) / 2;
      auto parent_cell = mArray[parent_pos];
      if ( parent_cell <= cur_cell ) {
	break;
      }
      mArray[parent_pos] = cur_cell;
      mArray[cur_pos] = parent_cell;
      cur_pos = parent_pos;
    }
  }

  /// @brief 下に移動させる．
  void
  move_down(
    SizeType cur_pos ///< [in] 現在の位置
  )
  {
    while ( true ) {
      auto cur_cell = mArray[cur_pos];
      auto right_pos = (cur_pos + 1) * 2;
      auto left_pos = right_pos - 1;
      if ( mArray.size() <= left_pos ) {
	break;
      }
      // より小さいサイズを持つ子供を見つける．
      auto child_cell = mArray[left_pos];
      auto child_pos = left_pos;
      if ( mArray.size() > right_pos ) {
	auto right_cell = mArray[right_pos];
	if ( child_cell > right_cell ) {
	  child_cell = right_cell;
	  child_pos = right_pos;
	}
      }
      // それと比較する．
      if ( cur_cell <= child_cell ) {
	break;
      }
      mArray[cur_pos] = child_cell;
      mArray[child_pos] = cur_cell;
      cur_pos = child_pos;
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列本体
  vector<Cell> mArray;

};

END_NAMESPACE_DRUID

#endif // BDDHEAP_H
