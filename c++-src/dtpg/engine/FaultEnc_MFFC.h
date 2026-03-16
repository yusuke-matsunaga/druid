#ifndef FAULTENC_MFFC_H
#define FAULTENC_MFFC_H

/// @file FaultEnc_MFFC.h
/// @brief FaultEnc_MFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultEnc.h"
#include "types/TpgMFFC.h"


BEGIN_NAMESPACE_DRUID

class MFFCEnc;

//////////////////////////////////////////////////////////////////////
/// @class FaultEnc_MFFC FaultEnc_MFFC.h "FaultEnc_MFFC.h"
/// @brief MFFC 内の故障伝搬条件を生成するクラス
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class FaultEnc_MFFC :
  public FaultEnc
{
public:

  /// @brief コンストラクタ
  FaultEnc_MFFC(
    BdEngine& engine,   ///< [in] エンジン
    const TpgMFFC& mffc ///< [in] 対象の MFFC
  );

  /// @brief デストラクタ
  ~FaultEnc_MFFC() = default;


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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // MFFC内のエンコーダ
  MFFCEnc* mEnc;

};

END_NAMESPACE_DRUID

#endif // FAULTENC_MFFC_H
