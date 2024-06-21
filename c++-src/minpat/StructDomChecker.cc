
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
  mEngine.add_simple_cone(fault1->origin_node(), true);
  mEngine.add_simple_cone(fault2->origin_node(), false);
  mEngine.make_cnf();
}

// @brief デストラクタ
StructDomChecker::~StructDomChecker()
{
}

// @brief チェックする．
bool
StructDomChecker::check()
{
  auto ex_cond1 = mFault1->excitation_condition();
  auto ex_cond2 = mFault2->excitation_condition();
  if ( mEngine.check_sat(ex_cond1, ex_cond2) == SatBool3::False ) {
    return true;
  }
  return false;
}

END_NAMESPACE_DRUID
