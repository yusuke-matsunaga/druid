
/// @file Dtpg_se.cc
/// @brief Dtpg_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Dtpg_se.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ(ノードモード)
Dtpg_se::Dtpg_se(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgNode* node,
  const string& just_type,
  const SatSolverType& solver_type
) : mStructEnc{network, fault_type, solver_type},
    mFaultType{fault_type},
    mJustifier{just_type, network},
    mTimerEnable{true}
{
  cnf_begin();

  mStructEnc.add_simple_cone(node->ffr_root(), true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end();
}


// @brief コンストラクタ(ffrモード)
Dtpg_se::Dtpg_se(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgFFR& ffr,
  const string& just_type,
  const SatSolverType& solver_type
) : mStructEnc{network, fault_type, solver_type},
    mFaultType{fault_type},
    mJustifier{just_type, network},
    mTimerEnable{true}
{
  cnf_begin();

  mStructEnc.add_simple_cone(ffr.root(), true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end();
}

// @brief コンストラクタ(mffcモード)
Dtpg_se::Dtpg_se(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgMFFC& mffc,
  const string& just_type,
  const SatSolverType& solver_type
) : mStructEnc{network, fault_type, solver_type},
    mFaultType{fault_type},
    mJustifier{just_type, network},
    mTimerEnable{true}
{
  cnf_begin();

  if ( mffc.ffr_num() > 1 ) {
    mStructEnc.add_mffc_cone(mffc, true);
  }
  else {
    mStructEnc.add_simple_cone(mffc.root(), true);
  }

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end();
}

// @brief デストラクタ
Dtpg_se::~Dtpg_se()
{
}

// @brief テスト生成を行なう．
DtpgResult
Dtpg_se::gen_pattern(
  const TpgFault* fault
)
{
  Timer timer;
  timer.start();

  auto prev_stats = mStructEnc.solver().get_stats();

  // 故障が属している FFR の根のノード
  auto ffr_root = fault->tpg_onode()->ffr_root();

  // FFR より出力側の故障伝搬条件を assumptions に入れる．
  auto assumptions = mStructEnc.make_prop_condition(ffr_root, 0);

  // FFR 内の故障伝搬条件を assign_list に入れる．
  auto assign_list = fault->ffr_propagate_condition(mStructEnc.fault_type());

  // assign_list を変換して assumptions に追加する．
  auto as2 = mStructEnc.conv_to_literal_list(assign_list);
  assumptions.insert(assumptions.end(), as2.begin(), as2.end());

  // SAT問題を解く
  auto ans = mStructEnc.solver().solve(assumptions);
  const auto& model = mStructEnc.solver().model();

  timer.stop();
  auto time = timer.get_time();

  auto sat_stats = mStructEnc.solver().get_stats();
  //sat_stats -= prev_stats;

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // ffr_root より先の伝搬条件を求める．
    auto assign_list2 = mStructEnc.extract_prop_condition(ffr_root, 0, model);
    assign_list.merge(assign_list2);

    // assign_list の条件を正当化する．
#if 0
    auto testvect = mStructEnc.justify(model, assign_list, mJustifier);
#else
    auto testvect = mJustifier(mStructEnc.fault_type(), assign_list,
			       mStructEnc.hvar_map(),
			       mStructEnc.gvar_map(),
			       model);
#endif

    timer.stop();
    mStats.mBackTraceTime += timer.get_time();
    mStats.update_det(sat_stats, time);
    return DtpgResult{testvect};
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_red(sat_stats, time);
    return DtpgResult{FaultStatus::Untestable};
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(sat_stats, time);
    return DtpgResult{FaultStatus::Undetected};
  }
}

// @brief DTPG の統計情報を返す．
const DtpgStats&
Dtpg_se::stats() const
{
  return mStats;
}

// @brief タイマーをスタートする．
void
Dtpg_se::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
Dtpg_se::cnf_end()
{
  auto time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
Dtpg_se::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
double
Dtpg_se::timer_stop()
{
  if ( mTimerEnable ) {
    mTimer.stop();
    return mTimer.get_time();
  }
  else {
    return 0.0;
  }
}

END_NAMESPACE_DRUID
