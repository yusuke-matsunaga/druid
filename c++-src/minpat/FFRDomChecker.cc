
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
  const JsonValue& option
) : mFFR{ffr},
    mBaseEnc{network, option}
{
  mBdEnc = new BoolDiffEnc{mBaseEnc, ffr->root(), option};
  mBaseEnc.make_cnf({}, {ffr->root()});
  mBaseEnc.solver().add_clause(mBdEnc->prop_var());
}

// @brief デストラクタ
FFRDomChecker::~FFRDomChecker()
{
}

// @brief チェックする．
void
FFRDomChecker::check(
  const TpgFault* fault1,
  const vector<const TpgFault*>& fault2_list,
  vector<bool>& del_mark
)
{
  // チェック結果を保持する辞書
  unordered_map<SatLiteral, bool> result_map;

  // fault1 の検出条件
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  // プレースホルダ
  assumptions.push_back(SatLiteral::X);
  for ( auto fault2: fault2_list ) {
    // fault2 の検出条件から fault1 の検出条件を引く．
    auto ffr_cond2 = fault2->ffr_propagate_condition();
    ffr_cond2.diff(ffr_cond1);
    // ffr_cond1 を満たしつつ ffr_cond2 を一つでも満たさない解があれば
    // fault1 が検出可能で fault2 が検出不可能な場合があるということ．
    // つまり fault1 は fault2 を「支配していない」．
    bool unsat = true;
    for ( auto nv: ffr_cond2 ) {
      auto lit1 = mBaseEnc.conv_to_literal(nv);
      bool res = true;
      if ( result_map.count(lit1) ) {
	res = result_map.at(lit1);
      }
      else {
	assumptions[assumptions.size() - 1] = ~lit1;
	if ( mBaseEnc.solver().solve(assumptions) == SatBool3::True ) {
	  res = false;
	}
	result_map.emplace(lit1, res);
      }
      if ( !res ) {
	unsat = false;
	break;
      }
    }
    if ( unsat ) {
      del_mark[fault2->id()] = true;
    }
  }
}

END_NAMESPACE_DRUID
