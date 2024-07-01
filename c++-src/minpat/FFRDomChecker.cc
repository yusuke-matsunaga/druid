
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
SizeType
FFRDomChecker::check(
  const TpgFault* fault1,
  const vector<const TpgFault*>& fault2_list,
  vector<bool>& del_mark
)
{
  // チェック結果を保持する辞書
  unordered_map<SatLiteral, bool> result_map;

  SizeType count = 0;

  // fault1 の検出条件
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mBaseEnc.conv_to_literal_list(ffr_cond1);
  // プレースホルダ
  assumptions.push_back(SatLiteral::X);
  for ( auto fault2: fault2_list ) {
    // fault2 の検出条件から fault1 の検出条件を引く．
    auto ffr_cond2 = fault2->ffr_propagate_condition();
    ffr_cond2.diff(ffr_cond1);
    // ffr_cond1 を満たしつつ ffr_cond2 を満たさない解があれば
    // fault1 が検出可能で fault2 が検出不可能な場合があるということ．
    // つまり fault1 は fault2 を「支配していない」．
    // ffr_cond2 の否定は普通の節になるが，これは以降のチェックにおいては
    // 邪魔な節なので制御変数 clit を加えて，今回のチェック時のみ
    // clit を 1 にしておく．
    // 以降は clit が自由変数なのでこの節は常に充足される．
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
