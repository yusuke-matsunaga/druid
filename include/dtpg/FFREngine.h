#ifndef FFRENGINE_H
#define FFRENGINE_H

/// @file FFREngine.h
/// @brief FFREngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

class FFREnc;

//////////////////////////////////////////////////////////////////////
/// @class FFREngine FFREngine.h "dtpg/FFREngine.h"
/// @brief FFREnc を一つ組み込んだ StructEngine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class FFREngine:
  public StructEngine
{
public:

  /// @brief コンストラクタ
  FFREngine(
    const TpgFFR& ffr,              ///< [in] 対象の FFR
    const TpgFaultList& fault_list, ///< [in] ffr 内の故障リスト
    const ConfigParam& option = {}  ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFREngine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ffr() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list() const;

  /// @brief ffr() から到達可能な外部出力の数を返す．
  SizeType
  output_num() const;

  /// @brief ffr() から到達可能な外部出力を返す．
  TpgNode
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 故障の検出条件を表すリテラルのリストを返す．
  const std::vector<SatLiteral>&
  dlits(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 故障の非検出条件を表すリテラルを返す．
  SatLiteral
  ulit(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief 伝搬変数
  SatLiteral
  prop_var() const;

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief SAT問題の解から十分条件を求める．
  SuffCond
  extract_sufficient_condition(
    const TpgFault& fault,             ///< [in] 対象の故障
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SAT問題の解から十分条件を求める．
  SuffCond
  extract_sufficient_condition(
    const TpgFault& fault,             ///< [in] 対象の故障
    SizeType pos,                      ///< [in] 出力番号 ( 0 <= pos < output_num() )
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief extract_sufficient_condition() の下請け関数
  SuffCond
  _extract_sub(
    const SuffCond& cond,  ///< [in] FFR の根から出力までの検出条件
    const TpgFault& fault, ///< [in] 対象の故障
    const SatModel& model  ///< [in] SAT問題の解
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 所有権は StructEngine が持つ．
  FFREnc* mEnc;

};

END_NAMESPACE_DRUID

#endif // FFRENGINE_H
