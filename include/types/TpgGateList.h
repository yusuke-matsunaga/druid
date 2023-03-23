#ifndef TPGGATELIST_H
#define TPGGATELIST_H

/// @file TpgGateList.h
/// @brief TpgGateList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgGate.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgGateIter TpgGateList.h "TpgGateList.h"
/// @brief TpgGateList の反復子
//////////////////////////////////////////////////////////////////////
class TpgGateIter
{
public:

  /// @brief コンストラクタ
  TpgGateIter(
    const TpgNetworkImpl* network,
    SizeType pos
  ) : mNetwork{network},
      mPos{pos}
  {
  }

  /// @brief デストラクタ
  ~TpgGateIter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  TpgGate
  operator*() const
  {
    return TpgGate{mNetwork, mPos};
  }

  /// @brief next 演算子
  TpgGateIter&
  operator++()
  {
    ++ mPos;
    return *this;
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgGateIter& right
  ) const
  {
    return mNetwork == right.mNetwork && mPos == right.mPos;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgGateIter& right
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
/// @class TpgGateList TpgGateList.h "TpgGateList.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class TpgGateList
{
public:

  using iterator = TpgGateIter;

public:

  /// @brief コンストラクタ
  TpgGateList(
    const TpgNetworkImpl* network, ///< [in] ネットワーク
    SizeType num                   ///< [in] 要素数
  ) : mNetwork{network},
      mNum{num}
  {
  }

  /// @brief デストラクタ
  ~TpgGateList() = default;


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

#endif // TPGGATELIST_H
