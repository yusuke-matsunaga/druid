
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


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFRDomChecker::FFRDomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr}
{
  mBdEnc = new BoolDiffEnc(ffr->root(), option);
  StructEngine::Builder builder;
  builder.add_subenc(mBdEnc);
  mEngine = builder.new_obj(network, option);
  mEngine->solver().add_clause(mBdEnc->prop_var());
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
  // fault1 の検出条件
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mEngine->conv_to_literal_list(ffr_cond1);
  // プレースホルダ
  assumptions.push_back(SatLiteral::X);
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
  auto clit = mEngine->new_variable();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(ffr_cond2.size() + 1);
  tmp_lits.push_back(~clit);
  for ( auto nv: ffr_cond2 ) {
    auto lit = mEngine->conv_to_literal(nv);
    tmp_lits.push_back(~lit);
  }
  mEngine->solver().add_clause(tmp_lits);
  assumptions[assumptions.size() - 1] = clit;
  return mEngine->solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
