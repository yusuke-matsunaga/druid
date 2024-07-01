﻿
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

// @brief FFRのみの故障伝搬条件でチェックする．
bool
DomChecker::check0()
{
  auto lit1 = mBdEnc1->prop_var();
  auto lit2 = mBdEnc2->prop_var();
  vector<SatLiteral> assumptions{lit1, ~lit2};
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
}

// @brief 事前チェックをする．
bool
DomChecker::precheck(
  const TpgFault* fault1
)
{
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(~mBdEnc2->prop_var());
  return mBaseEnc.solver().solve(assumptions) == SatBool3::False;
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
  assumptions.push_back(SatLiteral::X); // プレースホルダ
  SizeType count = 0;
  unordered_map<SatLiteral, bool> result_dict;
  for ( auto fault2: fault2_list ) {
    if ( del_mark[fault2->id()] ) {
      continue;
    }
    auto ffr_cond2 = fault2->ffr_propagate_condition();
    auto clit = mBaseEnc.solver().new_variable();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ffr_cond2.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: ffr_cond2 ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
    assumptions[assumptions.size() - 1] = clit;
    bool unsat = mBaseEnc.solver().solve(assumptions) == SatBool3::False;
    if ( unsat ) {
      del_mark[fault2->id()] = true;
      ++ count;
    }
  }
  return count;
}

END_NAMESPACE_DRUID
