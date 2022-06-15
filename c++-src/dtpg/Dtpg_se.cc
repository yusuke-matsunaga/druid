
/// @file Dtpg_se.cc
/// @brief Dtpg_se の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg_se.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ(ノードモード)
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] node 故障のあるノード
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] solver_type SATソルバの実装タイプ
Dtpg_se::Dtpg_se(const TpgNetwork& network,
		 FaultType fault_type,
		 const TpgNode* node,
		 const string& just_type,
		 const SatSolverType& solver_type) :
  mStructEnc(network, fault_type, solver_type),
  mFaultType(fault_type),
  mJustifier(just_type, network),
  mTimerEnable(true)
{
  cnf_begin();

  mStructEnc.add_simple_cone(node->ffr_root(), true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end();
}


// @brief コンストラクタ(ffrモード)
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] ffr 故障伝搬の起点となる FFR
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] solver_type SATソルバの実装タイプ
Dtpg_se::Dtpg_se(const TpgNetwork& network,
		 FaultType fault_type,
		 const TpgFFR& ffr,
		 const string& just_type,
		 const SatSolverType& solver_type) :
  mStructEnc(network, fault_type, solver_type),
  mFaultType(fault_type),
  mJustifier(just_type, network),
  mTimerEnable(true)
{
  cnf_begin();

  mStructEnc.add_simple_cone(ffr.root(), true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end();
}

// @brief コンストラクタ(mffcモード)
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] mffc 故障伝搬の起点となる MFFC
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] solver_type SATソルバの実装タイプ
//
// この MFFC に含まれるすべての FFR が対象となる．
// FFR と MFFC が一致している場合は ffr モードと同じことになる．
Dtpg_se::Dtpg_se(const TpgNetwork& network,
		 FaultType fault_type,
		 const TpgMFFC& mffc,
		 const string& just_type,
		 const SatSolverType& solver_type) :
  mStructEnc(network, fault_type, solver_type),
  mFaultType(fault_type),
  mJustifier(just_type, network),
  mTimerEnable(true)
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
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで SatBool3::X を返す．
SatBool3
Dtpg_se::dtpg(const TpgFault* fault,
	      TestVector& testvect)
{
  Timer timer;
  timer.start();

  SatStats prev_stats;
  mStructEnc.solver().get_stats(prev_stats);

  vector<SatLiteral> assumptions;
  mStructEnc.make_fault_condition(fault, 0, assumptions);

  auto ans = mStructEnc.solver().solve(assumptions);
  const auto& model = mStructEnc.solver().model();

  timer.stop();
  auto time = timer.get_time();

  SatStats sat_stats;
  mStructEnc.solver().get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // バックトレースを行う．
    NodeValList assign_list = mStructEnc.extract(model, fault, 0);
    mStructEnc.justify(model, assign_list, mJustifier, testvect);

    timer.stop();
    mStats.mBackTraceTime += timer.get_time();
    mStats.update_det(sat_stats, time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_red(sat_stats, time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(sat_stats, time);
  }

  return ans;
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
