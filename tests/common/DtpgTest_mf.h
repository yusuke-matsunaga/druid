#ifndef DTPGTEST_FFR_H
#define DTPGTEST_FFR_H

/// @file DtpgTest_ffr.h
/// @brief DtpgTest_ffr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgTest_ffr DtpgTest_ffr.h "DtpgTest_ffr.h"
/// @brief Dtpg_se の ffr モードを使ったテスト
//////////////////////////////////////////////////////////////////////
class DtpgTest_ffr :
  public DtpgTest
{
public:

  /// @brief コンストラクタ
  DtpgTest_ffr(
    const TpgNetwork& network,       ///< [in] network 対象のネットワーク
    FaultType fault_type,	     ///< [in] fault_type 故障の種類
    const string& just_type,	     ///< [in] just_type Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] solver_type SATソルバのタイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~DtpgTest_ffr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストを行う．
  void
  _main_body() override;


};

END_NAMESPACE_DRUID

#endif // DTPGTEST_FFR_H
