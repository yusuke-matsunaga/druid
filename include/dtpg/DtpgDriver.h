#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

class DtpgStats;
class DtpgDriverImpl;

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver DtpgDriver.h "DtpgDriver.h"
/// @brief テスト生成を行う基底クラス
//////////////////////////////////////////////////////////////////////
class DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgDriver(
    DtpgDriverImpl* impl ///< [in] 実装クラス
  );

  /// @brief デストラクタ
  ~DtpgDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のテストパタンを求める．
  ///
  /// - f はコンストラクタで指定した FFR 内の故障でなければならない．
  void
  gen_pattern(
    const TpgFault* fault,         ///< [in] 対象の故障
    TpgFaultStatusMgr& status_mgr, ///< [in] 故障の状態を管理するオブジェクト
    DtpgStats& stats,              ///< [out] 統計情報
    FaultTvCallback det_func,      ///< [in] 検出時のコールバック関数
    FaultCallback untest_func,     ///< [in] 検出不能時のコールバック関数
    FaultCallback abort_func       ///< [in] アボート時のコールバック関数
  );

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラスのポインタ
  std::unique_ptr<DtpgDriverImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
