﻿
/// @file Dtpg2.cc
/// @brief Dtpg2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg2.h"

#include "Dtpg2Impl.h"
#include "TpgFaultMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "DopList.h"
#include "UntestOp.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"

#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
Dtpg2::Dtpg2(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt) :
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mBackTracer(bt),
  mImpl(nullptr)
{
}

// @brief デストラクタ
Dtpg2::~Dtpg2()
{
}

// @brief テスト生成を行う．
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレーター
// @param[in] network 対象のネットワーク
// @param[in] dop 故障検出時に実行されるファンクター
// @param[in] uop 故障が冗長故障の時に実行されるファンクター
// @param[inout] stats DTPGの統計情報
void
Dtpg2::run(TvMgr& tvmgr,
	   TpgFaultMgr& fmgr,
	   Fsim& fsim,
	   const TpgNetwork& network,
	   bool use_xorsampling,
	   DetectOp& dop,
	   UntestOp& uop,
	   DtpgStats& stats)
{
  ymuint wsa_limit = 0;
  { // 順序回路としてランダムに動かした時の
    // 平均の信号遷移回数を得る．
    ymuint count = 10000;
    ymuint warmup = 100;
    bool weighted = false;
    double wsa_ratio = 1.2;

    InputVector* i_vect = tvmgr.new_input_vector();
    DffVector* f_vect = tvmgr.new_dff_vector();
    double total_wsa = 0.0;
    RandGen rg;

    // 初期状態
    i_vect->set_from_random(rg);
    f_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *f_vect);

    for (ymuint i = 0; i < warmup; ++ i) {
      // このシミュレーション結果は捨てる．
      // 状態を遷移させることが目的
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, weighted);
    }
    for (ymuint i = 0; i < count; ++ i) {
      i_vect->set_from_random(rg);
      ymuint wsa1 = fsim.calc_wsa(*i_vect, weighted);
      total_wsa += wsa1;
    }
    double ave_wsa = total_wsa / count;

    tvmgr.delete_vector(i_vect);
    tvmgr.delete_vector(f_vect);

    wsa_limit = static_cast<ymuint>(ave_wsa * wsa_ratio);
  }

  ymuint nf = network.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      NodeValList nodeval_list;
      SatBool3 ans = dtpg(tvmgr, fsim, network, fault, use_xorsampling, wsa_limit,
			  nodeval_list, stats);
      if ( ans == kB3True ) {
	dop(fault, nodeval_list);
      }
      else if ( ans == kB3False ) {
	uop(fault);
      }
    }
  }
}

// @brief テスト生成を行なう．
// @param[in] network 対象のネットワーク
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
SatBool3
Dtpg2::dtpg(TvMgr& tvmgr,
	    Fsim& fsim,
	    const TpgNetwork& network,
	    const TpgFault* fault,
	    bool use_xorsampling,
	    ymuint wsa_limit,
	    NodeValList& nodeval_list,
	    DtpgStats& stats)
{
  Dtpg2Impl impl(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
  impl.gen_cnf(stats);

  SatBool3 ans = impl.dtpg(fault, nodeval_list, stats);
  if ( !use_xorsampling || ans != kB3True ) {
    return ans;
  }

  TestVector* tv = tvmgr.new_vector();

  tv->set_from_assign_list(nodeval_list);
  ymuint wsa = fsim.calc_wsa(tv, false);
  if ( wsa <= wsa_limit ) {
    return kB3True;
  }

  // 今の故障に関係のある PPI の数を数える．
  ymuint xor_num = impl.make_xor_list();
  if ( xor_num > 30 ) {
    xor_num -= 20;
  }
  else if ( xor_num > 20 ) {
    xor_num -= 10;
  }
  if ( xor_num > 5 ) {
    xor_num = 5;
  }
  // とりあえず xor_num ぐらいの制約をつける．

  RandGen randgen;
  ymuint count = 0;
  ymuint fcount = 0;
  for ( ; count < 50; ++ count) {
    Dtpg2Impl impl2(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
    impl2.gen_cnf(stats);
    impl2.make_xor_list();
    impl2.add_xor_constraint(xor_num, randgen);
    NodeValList nodeval_list1;
    SatBool3 ans = impl2.dtpg(fault, nodeval_list1, stats);
    if ( ans != kB3True ) {
      continue;
    }

    tv->set_from_assign_list(nodeval_list1);
    ymuint wsa = fsim.calc_wsa(tv, false);
    if ( wsa <= wsa_limit ) {
      nodeval_list = nodeval_list1;
      break;
    }
    ++ fcount;
    if ( fcount > 20 ) {
      break;
    }
  }
  {
    cout << fault->str() << ": wsa = " << wsa
	 << ", count = " << count << ", fcount = " << fcount << endl;
  }

  tvmgr.delete_vector(tv);

  return kB3True;
}

END_NAMESPACE_YM_SATPG
