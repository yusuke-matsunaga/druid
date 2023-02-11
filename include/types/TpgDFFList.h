#ifndef TPGDFFLIST_H
#define TPGDFFLIST_H

/// @file TpgDFFList.h
/// @brief TpgDFFList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgDFFIter TpgDFFIter.h "TpgDFFIter.h"
/// @brief TpgDFFList の反復子
//////////////////////////////////////////////////////////////////////
class TpgDFFIter
{
public:

  /// @brief コンストラクタ
  TpgDFFIter(
    vector<DFFImpl>::const_iterator iter
  ) : mIter{iter}
  {
  }

  /// @brief デストラクタ
  ~TpgDFFIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgDFF
  operator*() const;

  /// @brief next 演算子
  TpgDFFIter&
  operator++();

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgDFFIter& right
  ) const
  {
    return mIter == right.mIter;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgDFFIter& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本当の反復子
  vector<DFFImpl>::const_iterator mIter;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDFFList TpgDFFList.h "TpgDFFList.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class TpgDFFList
{
public:

  using iterator = TpgDFFIter;

public:

  /// @brief コンストラクタ
  TpgDFFList(
    const vector<DFFImpl>& dff_list ///< [in] 本当のリスト
  ) : mDffList{dff_list}
  {
  }

  /// @brief デストラクタ
  ~TpgDFFList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const
  {
    return iterator{mDffList.begin()};
  }

  /// @brief 末尾の反復子を返す．
  iterator
  end() const
  {
    return iterator{mDffList.end()};
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本当のリスト
  const vector<DFFImpl>& mDffList;

};

END_NAMESPACE_DRUID

#endif // TPGDFFLIST_H
