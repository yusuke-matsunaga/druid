#ifndef CONDGENMGR_H
#define CONDGENMGR_H

/// @file CondGenMgr.h
/// @brief CondGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenMgr CondGenMgr.h "CondGenMgr.h"
/// @brief 故障の検出条件を作るクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class CondGenMgr
{
public:

  /// @brief FFRの根の故障伝搬条件用のコールバック関数
  using RootCondCallback =
    std::function<void(const TpgFFR*,  ///< [in] 対象の FFR
		       const DetCond&, ///< [in] 伝搬条件
		       SizeType,       ///< [in] ループ回数
		       double)>;       ///< [in] 計算時間

  /// @brief 故障検出条件用のコールバック関数
  using FaultCondCallback =
    std::function<void(const TpgFault*, ///< [in] 対象の故障
		       const DetCond&,  ///< [in] 検出条件
		       SizeType,        ///< [in] ループ回数
		       double)>;        ///< [in] 計算時間

public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRの故障伝搬条件を求める．
  static
  void
  root_cond(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    SizeType limit,                      ///< [in] ループ回数の上限
    RootCondCallback callback,           ///< [in] コールバック関数
    const JsonValue& option              ///< [in] オプション
  );

  /// @brief 故障検出条件を求める．
  static
  void
  fault_cond(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障のリスト
    SizeType limit,                            ///< [in] ループ回数の上限
    FaultCondCallback callback,                ///< [in] コールバック関数
    const JsonValue& option                    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
