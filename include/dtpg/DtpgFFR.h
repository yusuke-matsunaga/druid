#ifndef DTPGFFR_H
#define DTPGFFR_H

/// @file DtpgFFR.h
/// @brief DtpgFFR のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgFFR DtpgFFR.h "DtpgFFR.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgFFR :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgFFR(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgFFR& ffr,		     ///< [in] 故障伝搬の起点となる FFR
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~DtpgFFR();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @return 結果を返す．
  ///
  /// * tv_list[0] は DtpgResult のベクタと同じ．
  /// * tv_list の要素数が k より少ない場合がある．
  DtpgResult
  gen_k_patterns(
    const TpgFault* fault,      ///< [in] 対象の故障
    int k,			///< [in] 求めるベクタ数
    vector<TestVector>& tv_list	///< [out] ベクタを入れるリスト
  );

  /// @brief テストパタンの核となる式を求める．
  /// @return テストパタンの核となる論理式
  ///
  /// 検出不能の場合は定数０が返される．
  Expr
  gen_core_expr(
    const TpgFault* fault, ///< [in] 対象の故障
    int k		   ///< [in] 繰り返し回数
  );

};

END_NAMESPACE_DRUID

#endif // DTPGFFR_H
