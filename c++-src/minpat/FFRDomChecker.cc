
/// @file FFRDomChecker.cc
/// @brief FFRDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFRDomChecker.h"

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
FFRDomChecker::FFRDomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const vector<const TpgFault*>& fault2_list,
  const JsonValue& option
) : mFFR{ffr},
    mFault2List{fault2_list},
    mBaseEnc{network, option},
    mTimerEnable{true}
{
  mBdEnc = new BoolDiffEnc{mBaseEnc, ffr->root(), option};
  mFFREnc = new FFREnc{mBaseEnc, nullptr, ffr, fault2_list};
  mBaseEnc.make_cnf({}, {ffr->root()});
  mBaseEnc.solver().add_clause(mBdEnc->prop_var());
}

// @brief デストラクタ
FFRDomChecker::~FFRDomChecker()
{
}

// @brief チェックする．
bool
FFRDomChecker::check(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  auto ffr_cond = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond);
  assumptions.push_back(~mFFREnc->prop_var(fault2));
  SatBool3 res = mBaseEnc.solver().solve(assumptions);

  return res == SatBool3::False;
}

END_NAMESPACE_DRUID
