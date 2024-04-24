
/// @file FFRStructEncDriver.cc
/// @brief FFRStructEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFRStructEncDriver.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FFRStructEncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FFRStructEncDriver::FFRStructEncDriver(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  const JsonValue& option
) : mStructEnc{network, option},
    mRoot{ffr->root()},
    mJustifier{network, option}
{
  mStructEnc.add_simple_cone(mRoot, true);
  mStructEnc.make_cnf();
}

// @brief デストラクタ
FFRStructEncDriver::~FFRStructEncDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
FFRStructEncDriver::solve(
  const TpgFault* fault
)
{
  auto assumptions = mStructEnc.make_prop_condition(mRoot);
  auto assign_list = fault->ffr_propagate_condition();
  return mStructEnc.check_sat(assumptions, assign_list);
}

// @brief テストパタン生成を行う．
TestVector
FFRStructEncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto& model = mStructEnc.solver().model();
  auto assign_list = fault->ffr_propagate_condition();
  auto assign_list2 = mStructEnc.extract_prop_condition(mRoot);
  assign_list.merge(assign_list2);
  return mJustifier(assign_list, mStructEnc.hvar_map(), mStructEnc.gvar_map(), model);
}

END_NAMESPACE_DRUID
