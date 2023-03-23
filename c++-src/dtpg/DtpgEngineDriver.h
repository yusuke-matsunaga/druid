#ifndef DTPGENGINEDRIVER_H
#define DTPGENGINEDRIVER_H

/// @file DtpgEngineDriver.h
/// @brief DtpgEngineDriver のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"


BEGIN_NAMESPACE_DRUID

class DtpgEngine;

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngineDriver DtpgEngineDriver.h "DtpgEngineDriver.h"
/// @brief FFR 単位で DTPG の基本的な処理を DtpgDriver の継承クラス
//////////////////////////////////////////////////////////////////////
class DtpgEngineDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgEngineDriver(
    DtpgMgr& mgr,                    ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
  ) : DtpgDriver{mgr, network, fault_type, just_type, solver_type}
  {
  }

  /// @brief デストラクタ
  ~DtpgEngineDriver() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  void
  gen_pattern(
    DtpgEngine& engine,   ///< [in] 回路構造をエンコードしたもの
    const TpgFault& fault ///< [in] 対象の故障
  );

};

END_NAMESPACE_DRUID

#endif // DTPGENGINEDRIVER_H
