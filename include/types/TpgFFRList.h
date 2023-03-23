#ifndef TPGFFRLIST_H
#define TPGFFRLIST_H

/// @file TpgFFRList.h
/// @brief TpgFFRList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFFRIter TpgFFRIter.h "TpgFFRIter.h"
/// @brief TpgFFRList の反復子
//////////////////////////////////////////////////////////////////////
class TpgFFRIter
{
public:

  /// @brief コンストラクタ
  TpgFFRIter(
    const TpgNetworkImpl* network,
    SizeType pos
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~TpgFFRIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgFFR
  operator*() const
  {
    return TpgFFR{mNetwork, mPos};
  }

  /// @brief next 演算子
  TpgFFRIter&
  operator++()
  {
    ++ mPos;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgFFRIter& right
  ) const
  {
    return mNetwork == right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgFFRIter& right
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
/// @class TpgFFRList TpgFFRList.h "TpgFFRList.h"
/// @brief TpgFFR のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFFRList
{
public:

  using iterator = TpgFFRIter;

public:

  /// @brief コンストラクタ
  TpgFFRList(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType num                   ///< [in] 要素数
  ) : mNetwork{network},
      mNum{num}
  {
  }

  /// @brief デストラクタ
  ~TpgFFRList() = default;


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

#endif // TPGFFRLIST_H
