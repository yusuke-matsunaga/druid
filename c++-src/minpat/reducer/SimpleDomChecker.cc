
/// @file SimpleDomChecker.cc
/// @brief SimpleDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimpleDomChecker.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
SimpleDomChecker::SimpleDomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const vector<const TpgFault*>& fault2_list,
  const JsonValue& option
) : mEngine{network, option}
{
  mBdEnc1 = new BoolDiffEnc{mEngine, ffr1->root(), option};
  vector<const TpgNode*> tmp_list;
  tmp_list.push_back(ffr1->root());
  for ( auto fault: fault2_list ) {
    tmp_list.push_back(fault->ffr_root());
  }
  mEngine.make_cnf(tmp_list, tmp_list);
}

// @brief デストラクタ
SimpleDomChecker::~SimpleDomChecker()
{
}

// @brief チェックする．
bool
SimpleDomChecker::check(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  ASSERT_COND( fault1->ffr_root() == mBdEnc1->root_node() );
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mEngine.conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(SatLiteral::X); // プレースホルダ
  auto ffr_cond2 = fault2->ffr_propagate_condition();
  for ( auto nv: ffr_cond2 ) {
    auto lit = mEngine.conv_to_literal(nv);
    assumptions.back() = ~lit;
    if ( mEngine.solver().solve(assumptions) != SatBool3::False ) {
      return false;
    }
  }
  return true;
}

END_NAMESPACE_DRUID
