
/// @file DtpgEngine2.cc
/// @brief DtpgEngine2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine2.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "GateEnc.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "TpgNodeSet.h"
#include "extract.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/Range.h"
#include "ym/Timer.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout

BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
const int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

SatInitParam
get_init_param(
  const JsonValue& option
)
{
  if ( option.is_object() ) {
    if ( option.has_key("sat_param") ) {
      auto val = option.at("sat_param");
      return SatInitParam{val};
    }
  }
  // デフォルト値
  return SatInitParam{};
}

JsonValue
get_ex_opt(
  const JsonValue& option
)
{
  if ( option.is_object() ) {
    const char* key = "extractor";
    if ( option.has_key(key) ) {
      return option.at(key);
    }
  }
  // デフォルト値(null)
  return JsonValue::null();
}

JsonValue
get_just_opt(
  const JsonValue& option
)
{
  if ( option.is_object() ) {
    const char* key = "justifier";
    if ( option.has_key(key) ) {
      return option.at(key);
    }
  }
  // デフォルト値(null)
  return JsonValue::null();
}

END_NONAMESPACE

// @brief コンストラクタ
DtpgEngine2::DtpgEngine2(
  const TpgNetwork& network,
  const TpgNode* root,
  const JsonValue& option
) : mSolver{get_init_param(option)},
    mExOpt{get_ex_opt(option)},
    mJustifier{network, get_just_opt(option)}
{
  make_cnf();
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
DtpgEngine2::conv_to_literal(
  NodeVal node_val
)
{
  auto node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
  return vid * inv;
}

// @brief 値割り当てをリテラルのリストに変換する．
void
DtpgEngine2::add_to_literal_list(
  const NodeValList& assign_list,
  vector<SatLiteral>& lit_list
)
{
  SizeType n0 = lit_list.size();
  SizeType n = assign_list.size();
  lit_list.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    lit_list.push_back(lit);
  }
}

// @brief テストパタン生成を行う．
SatBool3
DtpgEngine2::solve(
  const TpgFault* fault
)
{
  // fault 用の追加条件
  auto assumptions = gen_assumptions(fault);
  // fault の活性化条件と FFR 内の伝搬条件を加える．
  auto ffr_cond = fault->ffr_propagate_condition();
  add_to_literal_list(ffr_cond, assumptions);
  return mSolver.solve(assumptions);
}

// @brief 十分条件を取り出す．
NodeValList
DtpgEngine2::get_sufficient_condition(
  const TpgFault* fault
)
{
  // FFR の根の先の伝搬条件
  const auto& model = mSolver.model();
  auto ffr_root = fault->ffr_root();
  auto suf_cond = extract_sufficient_condition(ffr_root,
					       mGvarMap, mFvarMap, model,
					       mExOpt);
  // FFR 内の伝搬条件を加える．
  auto ffr_cond = fault->ffr_propagate_condition();
  suf_cond.merge(ffr_cond);
  return suf_cond;
}

// @brief 十分条件からテストベクタを作る．
TestVector
DtpgEngine2::justify(
  const NodeValList& assign_list
)
{
  const auto& model = mSolver.model();
  return mJustifier(assign_list, mHvarMap, mGvarMap, model);
}

END_NAMESPACE_DRUID
