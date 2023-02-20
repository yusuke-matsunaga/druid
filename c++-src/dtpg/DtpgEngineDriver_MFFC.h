#ifndef DTPGENGINEDRIVER_MFFC_H
#define DTPGENGINEDRIVER_MFFC_H

/// @file DtpgEngineDriver_MFFC.h
/// @brief DtpgEngineDriver_MFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngineDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngineDriver_MFFC DtpgEngineDriver_MFFC.h "DtpgEngineDriver_MFFC.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgEngineDriver_MFFC :
  public DtpgEngineDriver
{
public:

  /// @brief コンストラクタ
  DtpgEngineDriver_MFFC(
    DtpgMgr& mgr,                    ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  ) : DtpgEngineDriver{mgr, network, fault_type, just_type, solver_type}
  {
  }

  /// @brief デストラクタ
  ~DtpgEngineDriver_MFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINEDRIVER_MFFC_H
