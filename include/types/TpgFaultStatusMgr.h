#ifndef TPGFAULTSTATUSMGR_H
#define TPGFAULTSTATUSMGR_H

/// @file TpgFaultStatusMgr.h
/// @brief TpgFaultStatusMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultStatus.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultStatusMgr TpgFaultStatusMgr.h "TpgFaultStatusMgr.h"
/// @brief 故障の状態を表すクラス
///
/// TpgFault そのものは TpgNetwork に所属しており const 属性だが，
/// テストベクタによっては検出されるかどうかの結果が変わるので
/// その状態を表すためのクラス
//////////////////////////////////////////////////////////////////////
class TpgFaultStatusMgr
{
public:

  /// @brief コンストラクタ
  ///
  /// 全て FaultStatus::Undetected で初期化される．
  TpgFaultStatusMgr(
    const vector<const TpgFault*>& fault_list ///< [in] 故障のリスト
  );

  /// @brief デストラクタ
  ~TpgFaultStatusMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを得る．
  const vector<const TpgFault*>&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 故障の状態を得る．
  FaultStatus
  get_status(
    const TpgFault* fault ///< [in] 故障
  ) const;

  /// @brief 故障を状態を設定する．
  void
  set_status(
    const TpgFault* fault, ///< [in] 故障
    FaultStatus status
  );

  /// @brief 対象の全故障数を得る．
  SizeType
  total_count() const
  {
    return mFaultList.size();
  }

  /// @brief 検出された故障数を得る．
  SizeType
  detected_count() const
  {
    return mDetCount;
  }

  /// @brief 検出不能故障数を得る．
  SizeType
  untestable_count() const
  {
    return mUntestCount;
  }

  /// @brief 未検出故障数を得る．
  SizeType
  remain_count() const
  {
    return mRemainCount;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障のリスト
  vector<const TpgFault*> mFaultList;

  // 故障の状態の配列
  vector<FaultStatus> mStatusArray;

  // 未検出故障数
  SizeType mRemainCount;

  // 検出故障数
  SizeType mDetCount;

  // 検出不能故障数
  SizeType mUntestCount;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTSTATUSMGR_H
