
/// @file NaiveDomChecker.cc
/// @brief NaiveDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveDomChecker.h"
#include "TpgNetwork.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
NaiveDomChecker::NaiveDomChecker(
  const TpgNetwork& network,
  const TpgFault* fault1,
  const TpgFault* fault2,
  const JsonValue& option
) : mFault1{fault1},
    mFault2{fault2}
{
  auto node1 = fault1->origin_node();
  mBdEnc1 = new BoolDiffEnc(node1, option);
  mFaultEnc1 = new FaultEnc(fault1);

  auto node2 = fault2->origin_node();
  mBdEnc2 = new BoolDiffEnc(node2, option);
  mFaultEnc2 = new FaultEnc(fault2);

  StructEngine::Builder builder;
  builder.add_subenc(mBdEnc1);
  builder.add_subenc(mFaultEnc1);
  builder.add_subenc(mBdEnc2);
  builder.add_subenc(mFaultEnc2);
  mEngine = builder.new_obj(network, option);

  // fault1 の検出条件を追加する．
  {
    auto pvar1 = mBdEnc1->prop_var();
    auto pvar2 = mFaultEnc1->prop_var();
    mEngine->solver().add_clause(pvar1);
    mEngine->solver().add_clause(pvar2);
  }
  // fault2 は検出しないので mBdEnc2->prop_var() か mFaultEnc2->prop_var()
  // のいずれかは false
  {
    auto pvar1 = mBdEnc2->prop_var();
    auto pvar2 = mFaultEnc2->prop_var();
    mEngine->solver().add_clause(~pvar1, ~pvar2);
  }
}

// @brief デストラクタ
NaiveDomChecker::~NaiveDomChecker()
{
}

// @brief チェックする．
bool
NaiveDomChecker::check()
{
  return mEngine->solve() == SatBool3::False;
}

END_NAMESPACE_DRUID
