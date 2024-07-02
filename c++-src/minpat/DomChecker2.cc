
/// @file DomChecker2.cc
/// @brief DomChecker2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker2.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"


//#define DEBUG_DTPG

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif

END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DomChecker2::DomChecker2(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const vector<const TpgFFR*>& ffr2_list,
  const JsonValue& option
) : mBaseEnc{network, option}
{
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, ffr1->root(), option};
  vector<const TpgNode*> root2_list;
  root2_list.reserve(ffr2_list.size() + 1);
  for ( auto ffr2: ffr2_list ) {
    auto root2 = ffr2->root();
    root2_list.push_back(root2);
  }
  mBdEnc2 = new BoolDiffEnc2{mBaseEnc, root2_list, option};
  root2_list.push_back(ffr1->root());
  mBaseEnc.make_cnf({}, root2_list);
}

// @brief デストラクタ
DomChecker2::~DomChecker2()
{
}

// @brief チェックをする．
bool
DomChecker2::check(
  const TpgFault* fault1,
  const TpgFFR* ffr2
)
{
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(~mBdEnc2->prop_var());
  auto assumptions2 = mBdEnc2->cvar_assumptions(ffr2->root());
  assumptions.insert(assumptions.end(),
		     assumptions2.begin(), assumptions2.end());
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
