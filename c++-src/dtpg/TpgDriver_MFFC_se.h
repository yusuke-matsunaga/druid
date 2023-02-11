#ifndef TPGDRIVER_MFFC_SE_H
#define TPGDRIVER_MFFC_SE_H

/// @file TpgDriver_MFFC_se.h
/// @brief TpgDriver_MFFC_se のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDriver_MFFC_se TpgDriver_MFFC_se.h "TpgDriver_MFFC_se.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class TpgDriver_MFFC_se :
  public TpgDriver
{
public:

  /// @brief コンストラクタ
  TpgDriver_MFFC_se(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~TpgDriver_MFFC_se() = default;


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

#endif // TPGDRIVER_MFFC_SE_H
