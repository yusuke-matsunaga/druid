
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
    mBaseEnc{network, option}
{
  unordered_map<SizeType, const TpgFFR*> ffr_map;
  for ( auto ffr: network.ffr_list() ) {
    auto node = ffr->root();
    ffr_map.emplace(node->id(), ffr);
  }
  auto node1 = fault1->ffr_root();
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, node1, option};
  auto ffr1 = ffr_map.at(node1->id());
  mFFREnc1 = new FFREnc{mBaseEnc, ffr1};
  mFaultEnc1 = new FaultEnc{mBaseEnc, fault1};

  auto node2 = fault2->ffr_root();
  mBdEnc2 = new BoolDiffEnc{mBaseEnc, node2, option};
  auto ffr2 = ffr_map.at(node2->id());
  mFFREnc2 = new FFREnc{mBaseEnc, ffr2};
  mFaultEnc2 = new FaultEnc{mBaseEnc, fault2};

  mBaseEnc.make_cnf({}, {node1, node2});

  // fault1 の検出条件を追加する．
  {
    auto pvar1 = mBdEnc1->prop_var();
    auto pvar2 = mFFREnc1->prop_var(fault1->origin_node());
    auto pvar3 = mFaultEnc1->prop_var();
    mBaseEnc.solver().add_clause(pvar1);
    mBaseEnc.solver().add_clause(pvar2);
    mBaseEnc.solver().add_clause(pvar3);
  }
  // fault2 は検出しないので mBdEnc2->prop_var() か
  // mFFREnc2->prop_var()，mFaultEnc2->prop_var()
  // のいずれかは false
  {
    auto pvar1 = mBdEnc2->prop_var();
    auto pvar2 = mFFREnc2->prop_var(fault2->origin_node());
    auto pvar3 = mFaultEnc2->prop_var();
    mBaseEnc.solver().add_clause(~pvar1, ~pvar2, ~pvar3);
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
  return mBaseEnc.solver().solve() == SatBool3::False;
}

END_NAMESPACE_DRUID
