
/// @file NaiveDualEngine.cc
/// @brief NaiveDualEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/NaiveDualEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgFault.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
NaiveDualEngine::NaiveDualEngine(
  const TpgFault& fault1,
  const TpgFault& fault2,
  const ConfigParam& option
) : mEngine(fault1.network(), option),
#if 0
    mEnc1{new BoolDiffEnc(fault1.origin_node(), option)},
    mEnc2{new BoolDiffEnc(fault2.origin_node(), option)}
#else
    mEnc1{new BoolDiffEnc(fault1.ffr_root(), option)},
    mEnc2{new BoolDiffEnc(fault2.ffr_root(), option)}
#endif
{
  mEngine.add_subenc(std::unique_ptr<SubEnc>{mEnc1});
  mEngine.add_subenc(std::unique_ptr<SubEnc>{mEnc2});

  {
#if 0
    auto cond = fault1.excitation_condition();
#else
    auto cond = fault1.ffr_propagate_condition();
#endif
    auto lits = mEngine.conv_to_literal_list(cond);
    auto pvar = mEnc1->prop_var();
    mDlits1.reserve(lits.size() + 1);
    mDlits1.insert(mDlits1.end(), lits.begin(), lits.end());
    mDlits1.push_back(pvar);
    mClit1 = solver().new_variable(true);
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 2);
    tmp_lits.push_back(~mClit1);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    tmp_lits.push_back(~pvar);
    solver().add_clause(tmp_lits);
  }
  {
#if 0
    auto cond = fault2.excitation_condition();
#else
    auto cond = fault2.ffr_propagate_condition();
#endif
    auto lits = mEngine.conv_to_literal_list(cond);
    auto pvar = mEnc2->prop_var();
    mDlits2.reserve(lits.size() + 1);
    mDlits2.insert(mDlits2.end(), lits.begin(), lits.end());
    mDlits2.push_back(pvar);
    mClit2 = solver().new_variable(true);
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(lits.size() + 2);
    tmp_lits.push_back(~mClit2);
    for ( auto lit: lits ) {
      tmp_lits.push_back(~lit);
    }
    tmp_lits.push_back(~pvar);
    solver().add_clause(tmp_lits);
  }
}

// @brief デストラクタ
NaiveDualEngine::~NaiveDualEngine()
{
}

// @brief 問題を解く
SatBool3
NaiveDualEngine::solve(
  bool detect1,
  bool detect2
)
{
  std::vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(mDlits1.size() + mDlits2.size());
  if ( detect1 ) {
    tmp_lits.insert(tmp_lits.end(), mDlits1.begin(), mDlits1.end());
  }
  else {
    tmp_lits.push_back(mClit1);
  }
  if ( detect2 ) {
    tmp_lits.insert(tmp_lits.end(), mDlits2.begin(), mDlits2.end());
  }
  else {
    tmp_lits.push_back(mClit2);
  }
  return solver().solve(tmp_lits);
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDualEngine::extract_sufficient_condition1(
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc1->extract_sufficient_condition(model, assign_list);
  return cond;
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDualEngine::extract_sufficient_condition1(
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc1->extract_sufficient_condition(pos, model, assign_list);
  return cond;
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDualEngine::extract_sufficient_condition2(
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc2->extract_sufficient_condition(model, assign_list);
  return cond;
}

// @brief SATの解から十分条件を得る．
SuffCond
NaiveDualEngine::extract_sufficient_condition2(
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  auto cond = mEnc2->extract_sufficient_condition(pos, model, assign_list);
  return cond;
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
AssignList
NaiveDualEngine::justify(
  const SuffCond& suff_cond,
  const SatModel& model,
  const AssignList& assign_list
)
{
  return mEngine.justify(suff_cond, model, assign_list);
}

// @brief 値割り当てを対応するリテラルに変換する．
SatLiteral
NaiveDualEngine::conv_to_literal(
  Assign assign
)
{
  return mEngine.conv_to_literal(assign);
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
std::vector<SatLiteral>
NaiveDualEngine::conv_to_literal_list(
  const AssignList& assign_list
)
{
  return mEngine.conv_to_literal_list(assign_list);
}

// @brief 現在の内部状態を得る．
SatStats
NaiveDualEngine::get_stats() const
{
  return mEngine.get_stats();
}

// @brief CNF の生成時間を返す．
double
NaiveDualEngine::cnf_time() const
{
  return mEngine.cnf_time();
}

END_NAMESPACE_DRUID
