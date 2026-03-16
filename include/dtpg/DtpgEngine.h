#ifndef DTPGENGINE_H
#define DTPGENGINE_H

/// @file DtpgEngine.h
/// @brief DtpgEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/BdEngine.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultEnc;

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine.h DtpgEngine.h "DtpgEngine.h"
/// @brief 故障の検出条件を表すCNF式を生成するクラス
/// @ingroup DtpgGroup
///
/// 具体的には BdEngine と FaultEnc の仲介を行うだけのクラス
//////////////////////////////////////////////////////////////////////
class DtpgEngine
{
public:

  /// @brief コンストラクタ(Nodeモード)
  DtpgEngine(
    const TpgNode& node,    ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief コンストラクタ(FFRモード)
  DtpgEngine(
    const TpgFFR& ffr,      ///< [in] 故障伝搬の対象のFFR
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief コンストラクタ(MFFCモード)
  DtpgEngine(
    const TpgMFFC& mffc,    ///< [in] 故障伝搬の対象のMFFC
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~DtpgEngine();


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

  /// @brief 故障を検出する条件を返す．
  std::vector<SatLiteral>
  make_detect_condition(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief SATの解から十分条件を得る．
  SuffCond
  extract_sufficient_condition(
    const TpgFault& fault, ///< [in] 対象の故障
    const SatModel& model  ///< [in] SATの解
  );

  /// @brief SATの解から十分条件を得る．
  SuffCond
  extract_sufficient_condition(
    const TpgFault& fault, ///< [in] 対象の故障
    SizeType pos,          ///< [in] 出力番号
    const SatModel& model  ///< [in] SATの解
  );

  /// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
  /// @return 外部入力の割り当てリストを返す．
  ///
  /// * 必要な値割り当てのみが記録される．
  AssignList
  justify(
    const SuffCond& suff_cond, ///< [in] 割当リスト
    const SatModel& model      ///< [in] SAT問題の解
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

  // 故障の検出条件のエンコーダ
  std::unique_ptr<FaultEnc> mFaultEnc;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE_H
