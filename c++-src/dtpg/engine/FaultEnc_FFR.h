#ifndef FAULTENC_FFR_H
#define FAULTENC_FFR_H

/// @file FaultEnc_FFR.h
/// @brief FaultEnc_FFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultEnc_FFR FaultEnc_FFR.h "dtpg/FaultEnc_FFR.h"
/// @brief FFR単位で故障の検出条件の生成を行うクラス
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class FaultEnc_FFR :
  public FaultEnc
{
public:

  /// @brief デストラクタ
  ~FaultEnc_FFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // FaultEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の活性化と伝搬条件の値割り当てを返す．
  AssignList
  fault_propagate_condition(
    const TpgFault& fault ///< [in] 対象の故障
  ) override;

  /// @brief 故障検出のための補助的な条件を追加する．
  void
  add_aux_condition(
    const TpgFault& fault,            ///< [in] 対象の故障
    std::vector<SatLiteral>& lit_list ///< [in] 条件を表すリテラルのリスト
  ) override;

  /// @brief SATの解から十分条件を得る．
  SuffCond
  extract_sufficient_condition(
    BdEngine& engine,      ///< [in] エンジン
    const TpgFault& fault, ///< [in] 対象の故障
    const SuffCond& cond,  ///< [in] 故障伝搬条件
    const SatModel& model  ///< [in] SATの解
  ) override;

};

END_NAMESPACE_DRUID

#endif // FAULTENC_FFR_H
