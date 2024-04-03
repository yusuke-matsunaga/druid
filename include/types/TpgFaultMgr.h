#ifndef TPGFAULTMGR_H
#define TPGFAULTMGR_H

/// @file TpgFaultMgr.h
/// @brief TpgFaultMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#error "obsolete"
#include "druid.h"
#include "FaultType.h"
#include "FaultStatus.h"
#include "TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultMgrImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr TpgFaultMgr.h "TpgFaultMgr.h"
/// @brief 故障を管理するクラス
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr
{
public:

  /// @brief コンストラクタ
  TpgFaultMgr();

  /// @brief デストラクタ
  ~TpgFaultMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のリストを作る．
  void
  gen_fault_list(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    FaultType fault_type       ///< [in] 故障の種類
  );

  /// @brief 故障番号から故障を得る．
  TpgFault
  fault(
    SizeType id ///< [in] 故障番号
  ) const;

  /// @brief 故障のタイプを返す．
  FaultType
  fault_type() const;

  /// @brief 全故障のリストを返す．
  TpgFaultList
  fault_list() const;

  /// @brief 全代表故障のリストを返す．
  TpgFaultList
  rep_fault_list() const;

  /// @brief ノードに関係する代表故障のリストを返す．
  TpgFaultList
  node_fault_list(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief FFR に関係する代表故障のリストを返す．
  TpgFaultList
  ffr_fault_list(
    SizeType ffr_id ///< [in] FFR番号
  ) const;

  /// @brief MFFC に関係する代表故障のリストを返す．
  TpgFaultList
  mffc_fault_list(
    SizeType mffc_id ///< [in] FFR番号
  ) const;

  /// @brief 故障の状態をセットする．
  void
  set_status(
    const TpgFault& fault, ///< [in] 故障
    FaultStatus status	   ///< [in] 故障の状態
  );

  /// @brief 故障の状態を得る．
  FaultStatus
  get_status(
    const TpgFault& fault ///< [in] 故障
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  unique_ptr<TpgFaultMgrImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTMGR_H
