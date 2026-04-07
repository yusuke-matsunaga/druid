#ifndef NAIVEDTPGENGINE_H
#define NAIVEDTPGENGINE_H

/// @file NaiveDtpgEngine.h
/// @brief NaiveDtpgEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/BdEngine.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

class FaultEnc;

//////////////////////////////////////////////////////////////////////
/// @class NaiveDtpgEngine.h NaiveDtpgEngine.h "NaiveDtpgEngine.h"
/// @brief 故障の検出条件を表すCNF式を生成するクラス
/// @ingroup DtpgGroup
///
/// DtpgEngine と異なり単一の故障に特化した条件を生成する．
/// 主にデバッグ目的のリファレンス実装
//////////////////////////////////////////////////////////////////////
class NaiveDtpgEngine
{
public:

  /// @brief コンストラクタ
  NaiveDtpgEngine(
    const TpgFault& fault,         ///< [in] 対象の故障
    const ConfigParam& option = {} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NaiveDtpgEngine();


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

  /// @brief SAT問題を解く
  SatBool3
  solve(
    const std::vector<SatLiteral>& assumptions = {} ///< [in] 仮定
  );

  /// @brief SATの解から十分条件を得る．
  SuffCond
  extract_sufficient_condition(
    const SatModel& model,             ///< [in] SATの解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SATの解から十分条件を得る．
  SuffCond
  extract_sufficient_condition(
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

  // BdEngine
  BdEngine mEngine;

};

END_NAMESPACE_DRUID

#endif // NAIVEDTPGENGINE_H
