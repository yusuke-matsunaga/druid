
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
  const JsonValue& option
) : mFFR1{ffr1},
    mFFR2{ffr2},
    mBaseEnc{network, option}
{
  mBdEnc1 = new BoolDiffEnc{mBaseEnc, ffr1->root(), option};
  mBdEnc2 = new BoolDiffEnc{mBaseEnc, ffr2->root(), option};
  mBaseEnc.make_cnf({}, {ffr1->root(), ffr2->root()});
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief チェックする．
SizeType
DomChecker::check(
  const TpgFault* fault1,
  const vector<const TpgFault*>& fault2_list,
  vector<bool>& del_mark
)
{
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(~mBdEnc2->prop_var());
  if ( mBaseEnc.solver().solve(assumptions) == SatBool3::True ) {
    // そもそも fault1 を検出して ffr2 の根の故障伝搬が行われない
    // fault2_list すべてが被支配故障ではない．
    return 0;
  }
  SizeType count = 0;
  // 以降は ffr2 の根から先の故障伝搬は行われる場合のみ
  unordered_map<SatLiteral, bool> result_dict;
  for ( auto fault2: fault2_list ) {
    if ( del_mark[fault2->id()] ) {
      continue;
    }
    auto ffr_cond2 = fault2->ffr_propagate_condition();
    auto lits = mBaseEnc.conv_to_literal_list(ffr_cond2);
    bool unsat = true;
    for ( auto nv: ffr_cond2 ) {
      // lit の否定を加えたSAT問題がすべてUNSATなら被支配故障
      auto lit = mBaseEnc.conv_to_literal(nv);
      bool res = true;
      if ( result_dict.count(lit) ) {
	res = result_dict.at(lit);
      }
      else {
	assumptions[assumptions.size() - 1] = ~lit;
	if ( mBaseEnc.solver().solve(assumptions) == SatBool3::True ) {
	  res = false;
	}
	result_dict.emplace(lit, res);
      }
      if ( res == false ) {
	unsat = false;
	break;
      }
    }
    if ( unsat ) {
      del_mark[fault2->id()] = true;
      ++ count;
    }
  }
  return count;
}

END_NAMESPACE_DRUID
