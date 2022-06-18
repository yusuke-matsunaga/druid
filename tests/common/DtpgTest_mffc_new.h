#ifndef DTPGTEST_MFFC_NEW_H
#define DTPGTEST_MFFC_NEW_H

/// @file DtpgTest_mffc_new.h
/// @brief DtpgTest_mffc_new のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgTest_mffc_new DtpgTest_mffc_new.h "DtpgTest_mffc_new.h"
/// @brief DtpgMFFCを使ったテスト
//////////////////////////////////////////////////////////////////////
class DtpgTest_mffc_new :
  public DtpgTest
{
public:

  /// @brief コンストラクタ
  DtpgTest_mffc_new(
    const TpgNetwork& network,       ///< [in] network 対象のネットワーク
    FaultType fault_type,	     ///< [in] fault_type 故障の種類
    const string& just_type,	     ///< [in] just_type Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] solver_type SATソルバのタイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~DtpgTest_mffc_new();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストを行う．
  void
  _main_body() override;


};

END_NAMESPACE_DRUID

#endif // DTPGTEST_MFFC_NEW_H
