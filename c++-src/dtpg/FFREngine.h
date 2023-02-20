#ifndef FFRENGINE_H
#define FFRENGINE_H

/// @file FFREngine.h
/// @brief FFREngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFREngine FFREngine.h "FFREngine.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class FFREngine :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  FFREngine(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgFFR& ffr,	             ///< [in] 故障伝搬の起点となる FFR
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
  );

  /// @brief デストラクタ
  ~FFREngine();

#if 0
protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の追加処理
  void
  opt_make_cnf() override;

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;
#endif

};

END_NAMESPACE_DRUID

#endif // FFRENGINE_H
