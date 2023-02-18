#ifndef DTPGDRIVER_FFR_SE_H
#define DTPGDRIVER_FFR_SE_H

/// @file DtpgDriver_FFR_se.h
/// @brief DtpgDriver_FFR_se のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver_se.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_FFR_se DtpgDriver_FFR_se.h "DtpgDriver_FFR_se.h"
/// @brief FFR モードの DtpgDriver_se
//////////////////////////////////////////////////////////////////////
class DtpgDriver_FFR_se :
  public DtpgDriver_se
{
public:

  /// @brief コンストラクタ
  DtpgDriver_FFR_se(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~DtpgDriver_FFR_se() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_FFR_SE_H
