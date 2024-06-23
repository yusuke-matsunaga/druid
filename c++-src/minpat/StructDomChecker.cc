
/// @file StructDomChecker.cc
/// @brief StructDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "StructDomChecker.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
StructDomChecker::StructDomChecker(
  const TpgNetwork& network,
  const TpgFault* fault1,
  const TpgFault* fault2,
  const JsonValue& option
) : mEngine{network, option},
    mFault1{fault1},
    mFault2{fault2}
{
  mVar1 = mEngine.add_simple_cone(fault1->origin_node());
  mVar2 = mEngine.add_simple_cone(fault2->origin_node());
  mEngine.make_cnf();
  if ( 0 ) {
    auto ex_cond1 = mFault1->excitation_condition();
    auto exlit = mEngine.solver().new_variable();
    auto tmp_lits = mEngine.conv_to_literal_list(ex_cond1);
    mEngine.solver().add_andgate(exlit, tmp_lits);
    mEngine.solver().add_clause(mVar1);
    mEngine.solver().add_clause(exlit);
  }
  if ( 0 ) {
    auto ex_cond2 = mFault2->excitation_condition();
    auto exlit = mEngine.solver().new_variable();
    auto tmp_lits = mEngine.conv_to_literal_list(ex_cond2);
    mEngine.solver().add_andgate(exlit, tmp_lits);
    mEngine.solver().add_clause(~mVar2, ~exlit);
  }
}

// @brief デストラクタ
StructDomChecker::~StructDomChecker()
{
}

// @brief チェックする．
bool
StructDomChecker::check()
{
#if 0
  auto assign_list = mFault1->excitation_condition();
  if ( mEngine.check_sat(assign_list) == SatBool3::False ) {
    return true;
  }
#else
  if ( mEngine.check_sat() == SatBool3::False ) {
    return true;
  }
#endif
  return false;
}

END_NAMESPACE_DRUID
