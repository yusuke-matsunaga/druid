
/// @file Bd2Engine.cc
/// @brief Bd2Engine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/Bd2Engine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Bd2Engine
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Bd2Engine::Bd2Engine(
  const TpgNode& node1,
  const TpgNode& node2,
  const ConfigParam& option
) : StructEngine(node1.network(), option)
{
  auto mffc_root1 = node1.mffc_root();
  auto mffc_root2 = node2.mffc_root();
  if ( mffc_root1 == mffc_root2 ) {
    // 同じMFFCに属している場合
    auto enc0 = new BoolDiffEnc(mffc_root1, option);
    add_subenc(std::unique_ptr<SubEnc>{enc0});
    auto pvar = enc0->prop_var();
    solver().add_clause(pvar);
    mBdEnc1 = new BoolDiffEnc(node1, mffc_root1, option);
    mBdEnc2 = new BoolDiffEnc(node2, mffc_root1, option);
  }
  else {
    mBdEnc1 = new BoolDiffEnc(node1, option);
    mBdEnc2 = new BoolDiffEnc(node2, option);
  }
  add_subenc(std::unique_ptr<SubEnc>{mBdEnc1});
  add_subenc(std::unique_ptr<SubEnc>{mBdEnc2});
}

// @brief root_node1() から到達可能な外部出力のリストを返す．
const TpgNodeList&
Bd2Engine::output_list1() const
{
  return mBdEnc1->output_list();
}

// @brief root_node1() から到達可能な外部出力の数を返す．
SizeType
Bd2Engine::output_num1() const
{
  return mBdEnc1->output_num();
}

// @brief root_node1() から到達可能な外部出力を返す．
TpgNode
Bd2Engine::output1(
  SizeType pos
) const
{
  return mBdEnc1->output(pos);
}

// @brief root_node1() の伝搬変数
SatLiteral
Bd2Engine::prop_var1() const
{
  return mBdEnc1->prop_var();
}

// @brief root_node1() の微分結果を表す変数を返す．
SatLiteral
Bd2Engine::prop_var1(
  SizeType pos
) const
{
  return mBdEnc1->prop_var(pos);
}

// @brief root_node2() から到達可能な外部出力のリストを返す．
const TpgNodeList&
Bd2Engine::output_list2() const
{
  return mBdEnc2->output_list();
}

// @brief root_node2() から到達可能な外部出力の数を返す．
SizeType
Bd2Engine::output_num2() const
{
  return mBdEnc2->output_num();
}

// @brief root_node2() から到達可能な外部出力を返す．
TpgNode
Bd2Engine::output2(
  SizeType pos
) const
{
  return mBdEnc2->output(pos);
}

// @brief root_node2() の伝搬変数
SatLiteral
Bd2Engine::prop_var2() const
{
  return mBdEnc2->prop_var();
}

// @brief root_node2() の微分結果を表す変数を返す．
SatLiteral
Bd2Engine::prop_var2(
  SizeType pos
) const
{
  return mBdEnc2->prop_var(pos);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
Bd2Engine::extract_sufficient_condition(
  const SatModel& model,
  const AssignList& assign_list
)
{
  return mBdEnc1->extract_sufficient_condition(model, assign_list);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
Bd2Engine::extract_sufficient_condition(
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  return mBdEnc1->extract_sufficient_condition(pos, model, assign_list);
}

END_NAMESPACE_DRUID
