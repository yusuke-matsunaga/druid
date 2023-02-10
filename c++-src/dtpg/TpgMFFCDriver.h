#ifndef TPGMFFCDRIVER_H
#define TPGMFFCDRIVER_H

/// @file TpgMFFCDriver.h
/// @brief TpgMFFCDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDriver.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFCDriver TpgMFFCDriver.h "TpgMFFCDriver.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class TpgMFFCDriver :
  public TpgDriver
{
public:

  /// @brief コンストラクタ
  TpgMFFCDriver(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~TpgMFFCDriver() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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

#endif // TPGMFFCDRIVER_H
