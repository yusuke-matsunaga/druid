
/// @file MFFCStructEncDriver.cc
/// @brief MFFCStructEncDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "MFFCStructEncDriver.h"
#include "TpgMFFC.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MFFCStructEncDriver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MFFCStructEncDriver::MFFCStructEncDriver(
  const TpgNetwork& network,
  const TpgMFFC* mffc,
  const JsonValue& option
) : mStructEnc{network, option},
    mRoot{mffc->root()},
    mJustifier{network, option}
{
  mStructEnc.add_mffc_cone(mffc, true);
  mStructEnc.make_cnf();
}

// @brief デストラクタ
MFFCStructEncDriver::~MFFCStructEncDriver()
{
}

// @brief 故障を検出する条件を求める．
SatBool3
MFFCStructEncDriver::solve(
  const TpgFault* fault
)
{
  auto root = fault->ffr_root();
  auto assumptions = mStructEnc.make_prop_condition(root);
  auto assign_list = fault->ffr_propagate_condition();
  return mStructEnc.check_sat(assumptions, assign_list);
}

// @brief テストパタン生成を行う．
TestVector
MFFCStructEncDriver::gen_pattern(
  const TpgFault* fault
)
{
  auto root = fault->ffr_root();
  auto& model = mStructEnc.solver().model();
  auto assign_list = fault->ffr_propagate_condition();
  auto assign_list2 = mStructEnc.extract_prop_condition(root);
  assign_list.merge(assign_list2);
  return mJustifier(assign_list, mStructEnc.hvar_map(), mStructEnc.gvar_map(), model);
}

END_NAMESPACE_DRUID
