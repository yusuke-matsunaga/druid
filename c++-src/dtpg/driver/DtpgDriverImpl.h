#ifndef DTPGDRIVERIMPL_H
#define DTPGDRIVERIMPL_H

/// @file DtpgDriverImpl.h
/// @brief DtpgDriverImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriverImpl DtpgDriverImpl.h "DtpgDriverImpl.h"
/// @brief テストパタン生成を行うクラス
///
/// インターフェイスを定義するだけの純粋仮想クラス
//////////////////////////////////////////////////////////////////////
class DtpgDriverImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~DtpgDriverImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - fault はコンストラクタで指定した FFR 内の故障でなければならない．
  virtual
  SatBool3
  solve(
    const TpgFault& fault ///< [in] 対象の故障
  ) = 0;

  /// @brief 故障に対する処理を行う．
  virtual
  void
  fault_op(
    const TpgFault& fault, ///< [in] 対象の故障
    DtpgResults& results   ///< [in] 結果を格納するオブジェクト
  ) = 0;

  /// @brief CNF の生成時間を返す．
  virtual
  double
  cnf_time() const = 0;

  /// @brief SATの統計情報を返す．
  virtual
  SatStats
  sat_stats() const = 0;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVERIMPL_H
