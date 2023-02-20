#ifndef DTPGFFR_H
#define DTPGFFR_H

/// @file DtpgFFR.h
/// @brief DtpgFFR のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine2.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgFFR DtpgFFR.h "DtpgFFR.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgFFR :
  public DtpgEngine2
{
public:

  /// @brief コンストラクタ
  DtpgFFR(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgFFR& ffr,		     ///< [in] 故障伝搬の起点となる FFR
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
  );

  /// @brief デストラクタ
  ~DtpgFFR() = default;

};

END_NAMESPACE_DRUID

#endif // DTPGFFR_H
