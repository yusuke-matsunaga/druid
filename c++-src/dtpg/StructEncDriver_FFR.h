#ifndef STRUCTENCDRIVER_FFR_H
#define STRUCTENCDRIVER_FFR_H

/// @file StructEncDriver_FFR.h
/// @brief StructEncDriver_FFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "StructEncDriver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver_FFR StructEncDriver_FFR.h "StructEncDriver_FFR.h"
/// @brief FFR モードの DtpgDriver_se
//////////////////////////////////////////////////////////////////////
class StructEncDriver_FFR :
  public StructEncDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver_FFR(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  ) : StructEncDriver{mgr, network, fault_type, just_type, solver_type}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver_FFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // STRUCTENCDRIVER_FFR_H
