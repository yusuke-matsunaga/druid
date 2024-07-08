
/// @file ExCubeGen.cc
/// @brief ExCubeGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExCubeGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TestVector.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
ExCubeGen::ExCubeGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mBaseEnc{network, option}
{
  mBdEnc = new BoolDiffEnc{mBaseEnc, ffr->root(), option};
  mBaseEnc.make_cnf({}, {ffr->root()});
}

// @brief デストラクタ
ExCubeGen::~ExCubeGen()
{
}

// @brief 与えられた故障を検出するテストキューブを生成する．
void
ExCubeGen::run(
  FaultInfo& fault_info
)
{
  if ( fault_info.is_trivial() ) {
    // trivial ならなにもやることはない．
    return;
  }
  auto fault = fault_info.fault();
  if ( fault->ffr_root() != mFFR->root() ) {
    ostringstream buf;
    buf << fault->str() << " is not in the FFR";
    throw std::invalid_argument{buf.str()};
  }
  auto& mand_cond = fault_info.mandatory_condition();
  auto suff_cond = fault_info.sufficient_conditions().front();
  while ( true ) {
    suff_cond.diff(mand_cond);
    // suff_cond を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    auto clit = mBaseEnc.solver().new_variable(false);
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(suff_cond.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: suff_cond ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
    auto assumptions = mBaseEnc.conv_to_literal_list(mand_cond);
    assumptions.push_back(clit);
    auto res = mBaseEnc.solver().solve(assumptions);
    if ( res != SatBool3::True ) {
      break;
    }
    suff_cond = mBdEnc->extract_sufficient_condition();
    fault_info.add_sufficient_condition(suff_cond);
  }
}

END_NAMESPACE_DRUID
