#ifndef FAULTINFOMGR_H
#define FAULTINFOMGR_H

/// @file FaultInfoMgr.h
/// @brief FaultInfoMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class TpgNetwork;

//////////////////////////////////////////////////////////////////////
/// @class FaultInfoMgr FaultInfoMgr.h "FaultInfoMgr.h"
/// @brief FaultInfo を管理するクラス
//////////////////////////////////////////////////////////////////////
class FaultInfoMgr
{
public:

  /// @brief コンストラクタ
  FaultInfoMgr(
    const TpgNetwork& network,                ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~FaultInfoMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障情報を求める．
  void
  generate(
    const JsonValue& option ///< [in] オプション
                            ///<      - 'sat_param': object SATソルバ用のパラメータ
                            ///<      - 'justifier': str    justifier用のパラメータ
                            ///<      - 'debug': bool         デバッグフラグ
  );

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 故障リストを返す．
  const vector<const TpgFault*>&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 現時点で残っている故障リストを返す．
  const vector<const TpgFault*>&
  active_fault_list() const;

  /// @brief 故障情報を返す．
  const FaultInfo&
  fault_info(
    const TpgFault* fault ///< [in] 故障
  ) const
  {
    return mFaultInfoArray[fault->id()];
  }

  /// @brief FFR の出力の故障伝搬の必要条件を返す．
  const AssignList&
  root_mandatory_condition(
    const TpgFFR* ffr
  ) const
  {
    return mRootMandCondArray[ffr->id()];
  }

  /// @brief 故障を削除する．
  void
  delete_fault(
    const TpgFault* fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障リスト
  // テスト不能故障，未検出故障も含む
  vector<const TpgFault*> mFaultList;

  // 現時点で処理対象の故障のリスト
  mutable
  vector<const TpgFault*> mActiveFaultList;

  // 残っている故障数
  SizeType mFaultNum{0};

  // FaultInfo の配列
  // キーは故障番号
  vector<FaultInfo> mFaultInfoArray;

  // FFR の出力の故障伝搬の必要条件の配列
  // キーは FFR 番号
  vector<AssignList> mRootMandCondArray;

};

END_NAMESPACE_DRUID

#endif // FAULTINFOMGR_H
