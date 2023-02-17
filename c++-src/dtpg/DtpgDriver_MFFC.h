#ifndef DTPGDRIVER_MFFC_H
#define DTPGDRIVER_MFFC_H

/// @file DtpgDriver_MFFC.h
/// @brief DtpgDriver_MFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_MFFC DtpgDriver_MFFC.h "DtpgDriver_MFFC.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgDriver_MFFC :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgDriver_MFFC(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~DtpgDriver_MFFC() = default;


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

#endif // DTPGDRIVER_MFFC_H
