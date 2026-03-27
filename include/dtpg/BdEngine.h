#ifndef BDENGINE_H
#define BDENGINE_H

/// @file BdEngine.h
/// @brief BdEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

class BoolDiffEnc;

//////////////////////////////////////////////////////////////////////
/// @class BdEngine BdEngine.h "dtpg/BdEngine.h"
/// @brief BoolDiffEnc を一つ組み込んだ StructEngine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class BdEngine:
  public StructEngine
{
public:

  /// @brief コンストラクタ
  BdEngine(
    const TpgNode& node,           ///< [in] 対象のノード
    const ConfigParam& option = {} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~BdEngine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief root_node() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list() const;

  /// @brief root_node() から到達可能な外部出力の数を返す．
  SizeType
  output_num() const;

  /// @brief root_node() から到達可能な外部出力を返す．
  TpgNode
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
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
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SAT問題の解から十分条件を求める．
  SuffCond
  extract_sufficient_condition(
    SizeType pos,                      ///< [in] 出力番号 ( 0 <= pos < output_num() )
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  BoolDiffEnc* mBdEnc;

};

END_NAMESPACE_DRUID

#endif // BDENGINE_H
