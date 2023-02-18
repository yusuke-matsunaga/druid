#ifndef DTPGDRIVER_SE_H
#define DTPGDRIVER_SE_H

/// @file DtpgDriver_se.h
/// @brief DtpgDriver_se のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"
#include "Justifier.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

class StructEnc;

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_se DtpgDriver_se.h "DtpgDriver_se.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgDriver_se :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgDriver_se(
    TpgMgr& mgr,                     ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デストラクタ
  ~DtpgDriver_se() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  void
  gen_pattern(
    StructEnc& enc,       ///< [in] 回路構造をエンコードしたもの
    const TpgFault* fault ///< [in] 対象の故障
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 正当化を行うファンクタ
  Justifier mJustifier;

  // SATソルバのタイプ
  SatSolverType mSolverType;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_SE_H
