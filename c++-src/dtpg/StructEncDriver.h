#ifndef STRUCTENCDRIVER_H
#define STRUCTENCDRIVER_H

/// @file StructEncDriver.h
/// @brief StructEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"


BEGIN_NAMESPACE_DRUID

class StructEnc;

//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver StructEncDriver.h "StructEncDriver.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class StructEncDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver(
    DtpgMgr& mgr,                    ///< [in] 親のマネージャ
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,            ///< [in] 故障の種類
    const string& just_type,         ///< [in] 正当化のタイプ
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  ) : DtpgDriver{mgr, network, fault_type, just_type, solver_type}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  void
  gen_pattern(
    StructEnc& enc,       ///< [in] 回路構造をエンコードしたもの
    const TpgFault& fault ///< [in] 対象の故障
  );

};

END_NAMESPACE_DRUID

#endif // STRUCTENCDRIVER_H
