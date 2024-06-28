
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"

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
DomChecker::DomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const TpgFFR* ffr2,
  const vector<const TpgFault*>& fault2_list,
  const JsonValue& option
) : mFFR1{ffr1},
    mFFR2{ffr2},
    mFault2List{fault2_list},
    mBaseEnc{network, option},
    mTimerEnable{true}
{
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, ffr1->root(), option};
  mBdEnc2 = new BoolDiffEnc{mBaseEnc, ffr2->root(), option};
  mFFREnc2 = new FFREnc{mBaseEnc, mBdEnc2, ffr2, fault2_list};
  mBaseEnc.make_cnf({}, {ffr1->root(), ffr2->root()});
  mBaseEnc.solver().add_clause(mBdEnc1->prop_var());
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief チェックする．
bool
DomChecker::check(
  const TpgFault* fault1,
  const TpgFault* fault2
)
{
  auto ffr_cond = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond);
  assumptions.push_back(~mFFREnc2->prop_var(fault2));
  SatBool3 res = mBaseEnc.solver().solve(assumptions);

  return res == SatBool3::False;
}

END_NAMESPACE_DRUID
