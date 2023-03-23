#ifndef TPGFAULTLIST_H
#define TPGFAULTLIST_H

/// @file TpgFaultList.h
/// @brief TpgFaultList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultMgrImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultIter TpgFaultList.h "TpgFaultList.h"
/// @brief TpgFaultList の反復子
//////////////////////////////////////////////////////////////////////
class TpgFaultIter
{
public:

  /// @brief コンストラクタ
  TpgFaultIter(
    TpgFaultMgrImpl* mgr,
    vector<SizeType>::const_iterator iter
  ) : mMgr{mgr},
      mIter{iter}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgFault
  operator*() const
  {
    return TpgFault{mMgr, *mIter};
  }

  /// @brief next 演算子
  TpgFaultIter&
  operator++()
  {
    ++ mIter;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgFaultIter& right
  ) const
  {
    return mMgr == right.mMgr && mIter == right.mIter;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgFaultIter& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障マネージャ
  TpgFaultMgrImpl* mMgr;

  // 番号のリストの反復子
  vector<SizeType>::const_iterator mIter;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFaultList TpgFaultList.h "TpgFaultList.h"
/// @brief TpgFault のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFaultList
{
public:

  using iterator = TpgFaultIter;

public:

  /// @brief コンストラクタ
  TpgFaultList(
    TpgFaultMgrImpl* mgr,           ///< [in] 故障マネージャ
    const vector<SizeType>& id_list ///< [in] ID番号のリスト
  ) : mMgr{mgr},
      mIdList{id_list}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 空の時 true を返す．
  bool
  empty() const
  {
    return size() == 0;
  }

  /// @brief 要素数を得る．
  SizeType
  size() const
  {
    return mIdList.size();
  }

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const
  {
    return iterator{mMgr, mIdList.begin()};
  }

  /// @brief 末尾の反復子を返す．
  iterator
  end() const
  {
    return iterator{mMgr, mIdList.end()};
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障マネージャ
  TpgFaultMgrImpl* mMgr;

  // ID番号のリスト
  vector<SizeType> mIdList;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTLIST_H
