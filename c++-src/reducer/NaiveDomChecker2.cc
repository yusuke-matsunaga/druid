
/// @file NaiveDomChecker2.cc
/// @brief NaiveDomChecker2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveDomChecker2.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
NaiveDomChecker2::NaiveDomChecker2(
  const TpgNetwork& network,
  const TpgFault* fault1,
  const TpgFault* fault2,
  const JsonValue& option
) : mFault1{fault1},
    mFault2{fault2},
    mEngine{network, option}
{
  unordered_map<SizeType, const TpgFFR*> ffr_map;
  for ( auto ffr: network.ffr_list() ) {
    auto node = ffr->root();
    ffr_map.emplace(node->id(), ffr);
  }

  auto node1 = fault1->ffr_root();
  {
    mBdEnc1 = new BoolDiffEnc{mEngine, node1, option};
  }
  auto node2 = fault2->ffr_root();
  {
    mBdEnc2 = new BoolDiffEnc{mEngine, node2, option};
    auto ffr2 = ffr_map.at(node2->id());
    mFFREnc2 = new FFREnc{mEngine, mBdEnc2, ffr2, {fault2}};
  }
  mEngine.make_cnf({}, {node1, node2});

  // fault1 の検出条件を追加する．
  {
    auto pvar1 = mBdEnc1->prop_var();
    mEngine.solver().add_clause(pvar1);
  }
  // fault2 は検出しないので mBdEnc2->prop_var() か
  // mFFREnc2->prop_var() のいずれかは false
  {
    auto pvar2 = mFFREnc2->prop_var(fault2);
    mEngine.solver().add_clause(~pvar2);
  }
}

// @brief デストラクタ
NaiveDomChecker2::~NaiveDomChecker2()
{
}

// @brief チェックする．
bool
NaiveDomChecker2::check()
{
  auto prop_cond = mFault1->ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(prop_cond);
  return mEngine.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
