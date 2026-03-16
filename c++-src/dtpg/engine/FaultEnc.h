#ifndef FAULTENC_H
#define FAULTENC_H

/// @file FaultEnc.h
/// @brief FaultEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultEnc.h FaultEnc.h "FaultEnc.h"
/// @brief 故障の検出条件を表すCNF式を生成するクラス
/// @ingroup DtpgGroup
///
/// このクラスはインターフェイス定義のみの純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class FaultEnc
{
public:

  /// @brief デストラクタ
  virtual
  ~FaultEnc() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の活性化と伝搬条件の値割り当てを返す．
  virtual
  AssignList
  fault_propagate_condition(
    const TpgFault& fault ///< [in] 対象の故障
  ) = 0;

  /// @brief 故障検出のための補助的な条件を追加する．
  virtual
  void
  add_aux_condition(
    const TpgFault& fault,            ///< [in] 対象の故障
    std::vector<SatLiteral>& lit_list ///< [in] 条件を表すリテラルのリスト
  ) = 0;

  /// @brief SATの解から十分条件を得る．
  virtual
  SuffCond
  extract_sufficient_condition(
    BdEngine& engine,      ///< [in] エンジン
    const TpgFault& fault, ///< [in] 対象の故障
    const SuffCond& cond,  ///< [in] 故障伝搬条件
    const SatModel& model  ///< [in] SATの解
  ) = 0;

};

END_NAMESPACE_DRUID

#endif // FAULTENC_H
