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
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const TpgFFR* ffr,	           ///< [in] 故障伝搬の起点となる FFR
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  );

  /// @brief デストラクタ
  ~FFREngine();


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

};

END_NAMESPACE_DRUID

#endif // FFRENGINE_H
