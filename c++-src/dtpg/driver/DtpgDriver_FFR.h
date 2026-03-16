#ifndef DTPGDRIVER_FFR_H
#define DTPGDRIVER_FFR_H

/// @file DtpgDriver_FFR.h
/// @brief DtpgDriver_FFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "dtpg/FFREngine.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver_FFR DtpgDriver_FFR.h "DtpgDriver_FFR.h"
/// @brief FFREngine を用いた DtpgDriverImpl
//////////////////////////////////////////////////////////////////////
class DtpgDriver_FFR :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  DtpgDriver_FFR(
    const TpgFFR& ffr,	    ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgDriver_FFR();


public:
  //////////////////////////////////////////////////////////////////////
  // DtpgDriverImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - fault はコンストラクタで指定した FFR 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault& fault ///< [in] 対象の故障
  ) override;

  /// @brief 故障に対する処理を行う．
  void
  fault_op(
    const TpgFault& fault, ///< [in] 対象の故障
    DtpgResults& result    ///< [in] 結果を格納するオブジェクト
  ) override;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const override;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFRエンジン
  FFREngine mEngine;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_FFRENC_H
