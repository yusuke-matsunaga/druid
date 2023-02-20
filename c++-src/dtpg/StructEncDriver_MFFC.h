#ifndef STRUCTENCDRIVER_MFFC_H
#define STRUCTENCDRIVER_MFFC_H

/// @file StructEncDriver_MFFC.h
/// @brief StructEncDriver_MFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "StructEncDriver.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver_MFFC StructEncDriver_MFFC.h "StructEncDriver_MFFC.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class StructEncDriver_MFFC :
  public StructEncDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver_MFFC(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  ) : StructEncDriver{mgr, network, fault_type, just_type, solver_type}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver_MFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // STRUCTENCDRIVER_MFFC_H
