#ifndef TPGMFFCLIST_H
#define TPGMFFCLIST_H

/// @file TpgMFFCList.h
/// @brief TpgMFFCList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFCIter TpgMFFCIter.h "TpgMFFCIter.h"
/// @brief TpgMFFCList の反復子
//////////////////////////////////////////////////////////////////////
class TpgMFFCIter
{
public:

  /// @brief コンストラクタ
  TpgMFFCIter(
    const TpgNetworkImpl* network,
    SizeType pos
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~TpgMFFCIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgMFFC
  operator*() const
  {
    return TpgMFFC{mNetwork, mPos};
  }

  /// @brief next 演算子
  TpgMFFCIter&
  operator++()
  {
    ++ mPos;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgMFFCIter& right
  ) const
  {
    return mNetwork == right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgMFFCIter& right
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetworkImpl* mNetwork;

  // 位置
  SizeType mPos;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgMFFCList TpgMFFCList.h "TpgMFFCList.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class TpgMFFCList
{
public:

  using iterator = TpgMFFCIter;

public:

  /// @brief コンストラクタ
  TpgMFFCList(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType num                   ///< [in] 要素数
  ) : mNetwork{network},
      mNum{num}
  {
  }

  /// @brief デストラクタ
  ~TpgMFFCList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const
  {
    return iterator{mNetwork, 0};
  }

  /// @brief 末尾の反復子を返す．
  iterator
  end() const
  {
    return iterator{mNetwork, mNum};
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetworkImpl* mNetwork;

  // 要素数
  SizeType mNum;

};

END_NAMESPACE_DRUID

#endif // TPGMFFCLIST_H
