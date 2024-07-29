#ifndef XCHECKER_H
#define XCHECKER_H

/// @file XChecker.h
/// @brief XChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class XChecker XChecker.h "XChecker.h"
/// @brief 2つのFFRが共通部分を持つかチェックするオブジェクト
//////////////////////////////////////////////////////////////////////
class XChecker
{
public:

  /// @brief コンストラクタ
  XChecker(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~XChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 2つの FFR が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFFR* ffr1,
    const TpgFFR* ffr2
  ) const;

  /// @brief 2つの故障が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFault* fault1,
    const TpgFault* fault2
  ) const;

  /// @brief 2つの故障が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFault* fault1,
    const TpgFFR* ffr2
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetwork& mNetwork;

  // 各FFRに関係する入力番号のリストの配列
  vector<vector<SizeType>> mInputListArray;

};

END_NAMESPACE_DRUID

#endif // XCHECKER_H
