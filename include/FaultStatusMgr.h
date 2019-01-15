#ifndef FAULTSTATUSMGR_H
#define FAULTSTATUSMGR_H

/// @file FaultStatusMgr.h
/// @brief FaultStatusMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "FaultStatus.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultStatusMgr FaultStatusMgr.h "FaultStatusMgr.h"
/// @brief 故障の状態を保持するクラス
//////////////////////////////////////////////////////////////////////
class FaultStatusMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  FaultStatusMgr(const TpgNetwork& network);

  /// @brief デストラクタ
  ~FaultStatusMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の状態をセットする．
  /// @param[in] fault 故障
  /// @param[in] status 故障の状態
  void
  set(const TpgFault* fault,
      FaultStatus status);

  /// @brief 故障の状態を得る．
  FaultStatus
  get(const TpgFault* fault) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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
