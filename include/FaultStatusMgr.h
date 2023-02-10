#ifndef FAULTSTATUSMGR_H
#define FAULTSTATUSMGR_H

/// @file FaultStatusMgr.h
/// @brief FaultStatusMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultStatus.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultStatusMgr FaultStatusMgr.h "FaultStatusMgr.h"
/// @brief 故障の状態を保持するクラス
///
/// 見かけは TpgFault をキーとした FaultStatus の辞書
/// ただし，常に全ての TpgFault の値を持っている．
//////////////////////////////////////////////////////////////////////
class FaultStatusMgr
{
public:

  /// @brief コンストラクタ
  FaultStatusMgr(
    const TpgNetwork& network  ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~FaultStatusMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の状態をセットする．
  void
  set(
    const TpgFault* fault, ///< [in] 故障
    FaultStatus status	   ///< [in] 故障の状態
  );

  /// @brief 故障の状態を得る．
  FaultStatus
  get(
    const TpgFault* fault ///< [in] 故障
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 各故障の状態を保持する配列
  // サイズは max_fault_id
  vector<FaultStatus> mStatusArray;

};

END_NAMESPACE_DRUID

#endif // FAULTSTATUSMGR_H
