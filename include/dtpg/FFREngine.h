#ifndef FFRENGINE_H
#define FFRENGINE_H

/// @file FFREngine.h
/// @brief FFREngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
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
    const TpgFFR* ffr,	           ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option        ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFREngine();


private:
  //////////////////////////////////////////////////////////////////////
  // DtpgEngine の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// このクラスでは空を返す．
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

};

END_NAMESPACE_DRUID

#endif // FFRENGINE_H
