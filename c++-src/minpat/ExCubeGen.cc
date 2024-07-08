
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

BEGIN_NONAMESPACE

JsonValue
get_dtpg_option(
  const JsonValue& option
)
{
  if ( option.is_object() && option.has_key("dtpg") ) {
    return option.get("dtpg");
  }
  return JsonValue{};
}

END_NONAMESPACE

// @brief コンストラクタ
ExCubeGen::ExCubeGen(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mFFR{ffr},
    mDtpgOption{get_dtpg_option(option)},
    mBaseEnc{network, mDtpgOption}
{
  JsonValue dtpg_option;
  if ( option.is_object() ) {
    if ( option.has_key("cube_per_fault") ) {
      mLimit = option.get("cube_per_fault").get_int();
    }
    if ( option.has_key("debug") ) {
      mDebug = option.get("debug").get_bool();
    }
  }
  mBdEnc = new BoolDiffEnc{mBaseEnc, ffr->root(), mDtpgOption};
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
  auto plit = mBdEnc->prop_var();
  auto clit = mBaseEnc.solver().new_variable(false);
  while ( fault_info.sufficient_conditions().size() < mLimit ) {
    suff_cond.diff(mand_cond);
    ASSERT_COND( suff_cond.size() > 0 );
    // suff_cond を否定した節を加える．
    // ただし他の故障の処理のときには無効化したいので
    // 制御変数をつけておく．
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(suff_cond.size() + 1);
    tmp_lits.push_back(~clit);
    for ( auto nv: suff_cond ) {
      auto lit = mBaseEnc.conv_to_literal(nv);
      tmp_lits.push_back(~lit);
    }
    mBaseEnc.solver().add_clause(tmp_lits);
    auto assumptions = mBaseEnc.conv_to_literal_list(mand_cond);
    assumptions.push_back(plit);
    assumptions.push_back(clit);
    auto res = mBaseEnc.solver().solve(assumptions);
    if ( res != SatBool3::True ) {
      cout << fault->str() << endl;
      if ( fault_info.sufficient_conditions().size() == 1 ) {
	cout << "  original sufficient condition: "
	     << fault_info.sufficient_conditions().front() << endl;
	cout << "  diff: " << suff_cond << endl;
      }
      break;
    }
    suff_cond = mBdEnc->extract_sufficient_condition();
    fault_info.add_sufficient_condition(suff_cond);
  }
  if ( mDebug ) {
    auto n = fault_info.sufficient_conditions().size();
    cout << fault->str()
	 << " ";
    if ( n == mLimit ) {
      cout << ">";
    }
    cout << n << " cubes" << endl;
  }
}

END_NAMESPACE_DRUID
