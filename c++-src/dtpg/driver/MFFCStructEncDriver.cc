
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
#include "ym/Timer.h"


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
    mJustifier{network, option.get("justifier")}
{
  Timer timer;
  timer.start();
  mStructEnc.add_mffc_cone(mffc, true);
  mStructEnc.make_cnf();
  timer.stop();
  mCnfTime = timer.get_time();
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
  auto pi_assign_list = mJustifier(assign_list, mStructEnc.hvar_map(),
				   mStructEnc.gvar_map(), model);
  return TestVector{mStructEnc.network(), pi_assign_list};
}

// @brief CNF の生成時間を返す．
double
MFFCStructEncDriver::cnf_time() const
{
  return mCnfTime;
}

// @brief SATの統計情報を返す．
SatStats
MFFCStructEncDriver::sat_stats() const
{
  return mStructEnc.sat_stats();
}

END_NAMESPACE_DRUID
