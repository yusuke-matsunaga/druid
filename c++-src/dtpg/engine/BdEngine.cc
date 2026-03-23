
/// @file BdEngine.cc
/// @brief BdEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/BdEngine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス BdEngine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BdEngine::BdEngine(
  const TpgNode& node,
  const ConfigParam& option
) : StructEngine(node.network(), option),
    mBdEnc{new BoolDiffEnc(node, option)}
{
  add_subenc(std::unique_ptr<SubEnc>{mBdEnc});
}

// @brief root_node() から到達可能な外部出力のリストを返す．
const TpgNodeList&
BdEngine::output_list() const
{
  return mBdEnc->output_list();
}

// @brief root_node() から到達可能な外部出力の数を返す．
SizeType
BdEngine::output_num() const
{
  return mBdEnc->output_num();
}

// @brief root_node() から到達可能な外部出力を返す．
TpgNode
BdEngine::output(
  SizeType pos
) const
{
  return mBdEnc->output(pos);
}

// @brief 伝搬変数
SatLiteral
BdEngine::prop_var() const
{
  return mBdEnc->prop_var();
}

// @brief 微分結果を表す変数を返す．
SatLiteral
BdEngine::prop_var(
  SizeType pos
) const
{
  return mBdEnc->prop_var(pos);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
BdEngine::extract_sufficient_condition(
  const SatModel& model
)
{
  return mBdEnc->extract_sufficient_condition(model);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
BdEngine::extract_sufficient_condition(
  SizeType pos,
  const SatModel& model
)
{
  return mBdEnc->extract_sufficient_condition(pos, model);
}

END_NAMESPACE_DRUID
