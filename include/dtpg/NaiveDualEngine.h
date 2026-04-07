#ifndef NAIVEDUALENGINE_H
#define NAIVEDUALENGINE_H

/// @file NaiveDualEngine.h
/// @brief NaiveDualEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

class BoolDiffEnc;

//////////////////////////////////////////////////////////////////////
/// @class NaiveDualEngine.h NaiveDualEngine.h "NaiveDualEngine.h"
/// @brief ２つの故障の検出条件を表すCNF式を生成するクラス
/// @ingroup DtpgGroup
///
/// １番目の故障は常に検出する条件を生成するが，２番めの故障に関しては
/// フラグに応じて検出条件と非検出条件を切り替える．
/// 主にデバッグ目的のリファレンス実装
//////////////////////////////////////////////////////////////////////
class NaiveDualEngine
{
public:

  /// @brief コンストラクタ
  NaiveDualEngine(
    const TpgFault& fault1,        ///< [in] 対象の故障1
    const TpgFault& fault2,        ///< [in] 対象の故障2
    const ConfigParam& option = {} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NaiveDualEngine();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mEngine.solver();
  }

  /// @brief 問題を解く
  SatBool3
  solve(
    bool detect1, ///< [in] fault1 を検出する時 true にする．
    bool detect2  ///< [in] fault2 を検出する時 true にする．
  );

  /// @brief SATの解から十分条件を得る(fault1用)．
  SuffCond
  extract_sufficient_condition1(
    const SatModel& model,             ///< [in] SATの解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SATの解から十分条件を得る(fault1用)．
  SuffCond
  extract_sufficient_condition1(
    SizeType pos,                      ///< [in] 出力番号
    const SatModel& model,             ///< [in] SATの解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SATの解から十分条件を得る(fault2用)．
  SuffCond
  extract_sufficient_condition2(
    const SatModel& model,             ///< [in] SATの解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SATの解から十分条件を得る(fault2用)．
  SuffCond
  extract_sufficient_condition2(
    SizeType pos,                      ///< [in] 出力番号
    const SatModel& model,             ///< [in] SATの解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
  /// @return 外部入力の割り当てリストを返す．
  ///
  /// * 必要な値割り当てのみが記録される．
  AssignList
  justify(
    const SuffCond& suff_cond,         ///< [in] 割当リスト
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief 値割り当てを対応するリテラルに変換する．
  SatLiteral
  conv_to_literal(
    Assign assign ///< [in] 値割り当て
  );

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  std::vector<SatLiteral>
  conv_to_literal_list(
    const AssignList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 構造エンジン
  StructEngine mEngine;

  // fault1 用のエンコーダー
  BoolDiffEnc* mEnc1;

  // fault2 用のエンコーダー
  BoolDiffEnc* mEnc2;

  // fault1 の検出条件
  std::vector<SatLiteral> mDlits1;

  // fault1 の非検出条件
  SatLiteral mClit1;

  // fault2 の検出条件
  std::vector<SatLiteral> mDlits2;

  // fault2 の非検出条件
  SatLiteral mClit2;

};

END_NAMESPACE_DRUID

#endif // NAIVEDUALENGINE_H
