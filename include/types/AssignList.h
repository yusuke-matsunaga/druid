#ifndef ASSIGNLIST_H
#define ASSIGNLIST_H

/// @file AssignList.h
/// @brief AssignList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"
#include "Assign.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AssignIter AssignList.h "AssignList.h"
/// @brief AssignList の反復子を表すクラス
/// @ingroup TypesGroup
/// @sa AssignList
//////////////////////////////////////////////////////////////////////
class AssignIter:
  public TpgBase
{
public:

  /// @brief 空のコンストラクタ
  AssignIter() = default;

  /// @brief 値を指定したコンストラクタ
  AssignIter(
    const std::shared_ptr<NetworkRep>& network,
    std::vector<SizeType>::const_iterator iter
  ) : TpgBase(network),
      mIter{iter}
  {
  }

  /// @brief デストラクタ
  ~AssignIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  Assign
  operator*() const
  {
    return Assign(_network(), *mIter);
  }

  /// @brief next 演算子
  AssignIter&
  operator++()
  {
    ++ mIter;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const AssignIter& right
  ) const
  {
    return _network() == right._network() && mIter == right.mIter;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const AssignIter& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 反復子の実体
  std::vector<SizeType>::const_iterator mIter;

};


//////////////////////////////////////////////////////////////////////
/// @class AssignList AssignList.h "td/AssignList.h"
/// @brief ノードに対する値の割当を記録するクラス
/// @ingroup TypesGroup
/// @sa Assign
///
/// * 概念的には vector<Assign> と同じ
/// * このクラスのメソッドはすべてソートされていると仮定している．
/// * 実際には内部で mDirty というフラグを用意して内容が変更されたら
///   mDirty を true にしておく．
///   内容を参照する際に mDirty が true なら _sort() を呼ぶ．
//////////////////////////////////////////////////////////////////////
class AssignList:
  public TpgBase
{
public:

  /// @brief コンストラクタ
  ///
  /// 不正な値となる．
  AssignList() = default;

  /// @brief 空のリストを作るコンストラクタ
  explicit
  AssignList(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    const std::vector<SizeType>& as_list        ///< [in] パックした割り当て情報のリスト
  ) : TpgBase(network),
      mDirty{false},
      mAsList{as_list}
  {
  }

  /// @brief コピーコンストラクタ
  AssignList(const AssignList& src) = default;

  /// @brief ムーブコンストラクタ
  AssignList(AssignList&& src) = default;

  /// @brief コピー代入演算子
  AssignList&
  operator=(const AssignList& src) = default;

  /// @brief ムーブ代入演算子
  AssignList&
  operator=(AssignList&& src) = default;

  /// @brief デストラクタ
  ~AssignList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  void
  clear()
  {
    mDirty = true;
    mAsList.clear();
  }

  /// @brief 値を追加する．
  void
  add(
    const TpgNode& node, ///< [in] ノード
    int time,	         ////< [in] 時刻 ( 0 or 1 )
    bool val	         ///< [in] 値
  )
  {
    if ( is_valid() ) {
      if ( !TpgBase::operator==(node) ) {
	throw std::invalid_argument{"network mismatch"};
      }
    }
    else {
      *(static_cast<TpgBase*>(this)) = node;
    }
    mAsList.push_back(Assign::encode(node.id(), time, val));
    mDirty = true;
  }

  /// @brief 値を追加する．
  void
  add(
    const Assign& assign ///< [in] 値の割り当て情報
  )
  {
    if ( is_valid() ) {
      if ( !TpgBase::operator==(assign) ) {
	throw std::invalid_argument{"network mismatch"};
      }
    }
    else {
      *(static_cast<TpgBase*>(this)) = assign;
    }
    mAsList.push_back(assign.mPackVal);
    mDirty = true;
  }

  /// @brief マージする．
  ///
  /// 矛盾する割当があった場合の動作は不定
  void
  merge(
    const AssignList& src_list  ///< [in] マージするリスト
  );

  /// @brief 差分を計算する．
  void
  diff(
    const AssignList& src_list  ///< [in] 差分の対象のリスト
  );

  /// @brief 差分を計算する．
  void
  diff(
    const Assign& src           ///< [in] 差分の対象
  );

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mAsList.size();
  }

  /// @brief 要素を返す．
  Assign
  elem(
    SizeType index  ///< [in] 位置 ( 0 <= index < size() )
  ) const
  {
    if ( index >= size() ) {
      throw std::out_of_range{"index is out of range"};
    }

    _sort();
    return Assign(_network(), mAsList[index]);
  }

  /// @brief add(Assign) の別名
  AssignList&
  operator+=(
    Assign node_val  ///< [in] 追加する要素
  )
  {
    add(node_val);

    return *this;
  }

  /// @brief merge() の別名
  AssignList&
  operator+=(
    const AssignList& src_list  ///< [in] 追加する要素のリスト
  )
  {
    merge(src_list);

    return *this;
  }

  /// @brief diff() の別名
  AssignList&
  operator-=(
    const AssignList& src_list  ///< [in] 差分の対象のリスト
  )
  {
    diff(src_list);

    return *this;
  }

  /// @brief diff() の別名
  AssignList&
  operator-=(
    const Assign& src  ///< [in] 差分の対象
  )
  {
    diff(src);

    return *this;
  }

  /// @brief elem() の別名
  Assign
  operator[](
    int pos  ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    return elem(pos);
  }

  /// @brief 矛盾した内容になっていないかチェックする．
  /// @return 正しければ true を返す．
  ///
  /// 具体的には同じノードで異なる値がある場合にエラーとなる．
  /// この関数はソートされている前提で動作する．
  bool
  sanity_check() const;

  /// @brief 先頭の反復子を返す．
  AssignIter
  begin() const
  {
    _sort();
    return AssignIter(_network(), mAsList.begin());
  }

  /// @brief 末尾の反復子を返す．
  AssignIter
  end() const
  {
    _sort();
    return AssignIter(_network(), mAsList.end());
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ソートされた状態にする．
  void
  _sort() const
  {
    if ( mDirty ) {
      // 重複した要素を削除する．
      std::sort(mAsList.begin(), mAsList.end());
      std::vector<SizeType> tmp_list;
      tmp_list.reserve(mAsList.size());
      SizeType prev = static_cast<SizeType>(-1);
      for ( auto cur: mAsList ) {
	if ( cur != prev ) {
	  tmp_list.push_back(cur);
	  prev = cur;
	}
      }
      std::swap(mAsList, tmp_list);
      mDirty = false;
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソートが必要かを表すフラグ
  mutable
  bool mDirty{false};

  // 値割り当てのリスト
  mutable
  std::vector<SizeType> mAsList;

};

/// @brief 2つの割当リストを比較する．
/// @retval -1 矛盾した割当がある．
/// @retval  0 無関係
/// @retval  1 src_list1 が src_list2 を含む．
/// @retval  2 src_list2 が src_list1 を含む．
/// @retval  3 等しい
int
compare(
  const AssignList& src_list1,
  const AssignList& src_list2
);

/// @brief 2つの割当リストが矛盾しているか調べる．
inline
bool
check_conflict(
  const AssignList& src_list1,
  const AssignList& src_list2
)
{
  return compare(src_list1, src_list2) == -1;
}

/// @brief 包含関係を調べる．
inline
bool
check_contain(
  const AssignList& src_list1,
  const AssignList& src_list2
)
{
  return (compare(src_list1, src_list2) & 1) == 1;
}

/// @brief 2つのリストを結合して新しいリストを返す．
inline
AssignList
operator+(
  const AssignList& src_list1,
  const AssignList& src_list2
)
{
  AssignList tmp(src_list1);
  return tmp.operator+=(src_list2);
}

/// @brief 2つのリストの差分を計算して新しいリストを返す．
inline
AssignList
operator-(
  const AssignList& src_list1,
  const AssignList& src_list2
)
{
  AssignList tmp(src_list1);
  return tmp.operator-=(src_list2);
}

/// @brief 2つのリストの差分を計算して新しいリストを返す．
inline
AssignList
operator-(
  const AssignList& src_list1,
  const Assign& src2
)
{
  AssignList tmp(src_list1);
  return tmp.operator-=(src2);
}

/// @brief 割当の内容を出力する．
/// @return s を返す．
std::ostream&
operator<<(
  std::ostream& s, ///< [in] 出力先のストリーム
  Assign nv        ///< [in] 値の割り当て
);

/// @brief 割当リストの内容を出力する．
/// @return s を返す．
std::ostream&
operator<<(
  std::ostream& s,           ///< [in] 出力先のストリーム
  const AssignList& src_list ///< [in] 値の割り当てリスト
);

END_NAMESPACE_DRUID

#endif // ASSIGNLIST_H
