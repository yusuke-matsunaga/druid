#ifndef CANDMGR_H
#define CANDMGR_H

/// @file CandMgr.h
/// @brief CandMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "FaultInfo.h"
#include "types/PackedVal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CandMgr CandMgr.h "CandMgr.h"
/// @brief 等価故障と支配故障候補のマネージャクラス
///
/// f1 が検出されて f2 が検出されないパタンがあった場合，
/// f1 が f2 を支配することはない．
/// この情報を管理するためのクラス
//////////////////////////////////////////////////////////////////////
class CandMgr
{
public:

  /// @brief コンストラクタ
  CandMgr(
    const FaultInfo& fault_info ///< [in] 対象の故障の情報
  ) : mFaultInfo{fault_info}
  {
  }

  /// @brief デストラクタ
  virtual
  ~CandMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  virtual
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) = 0;

  /// @brief 結果を返す．
  virtual
  EqDomCand
  end() = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障情報を返す．
  const FaultInfo&
  fault_info() const
  {
    return mFaultInfo;
  }

  /// @brief 対象の故障リストを返す．
  TpgFaultList
  fault_list() const
  {
    return mFaultInfo.fault_list();
  }

  /// @brief 対象のネットワークを返す．
  TpgNetwork
  network() const
  {
    return fault_list().network();
  }

  /// @brief 故障番号の最大値を返す．
  SizeType
  max_fault_size() const
  {
    return network().max_fault_id();
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障のリスト
  const FaultInfo& mFaultInfo;

};

END_NAMESPACE_DRUID

#endif // CANDMGR_H
