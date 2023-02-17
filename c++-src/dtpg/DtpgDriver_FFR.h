#ifndef DTPGDRIVER_FFR_H
#define DTPGDRIVER_FFR_H

/// @file DtpgDriver_FFR.h
/// @brief DtpgDriver_FFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_FFR DtpgDriver_FFR.h "DtpgDriver_FFR.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgDriver_FFR :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgDriver_FFR(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~DtpgDriver_FFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 正当化のタイプ
  string mJustType;

  // SATソルバのタイプ
  SatSolverType mSolverType;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_FFR_H
