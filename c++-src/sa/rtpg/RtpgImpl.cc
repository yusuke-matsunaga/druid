
/// @file RtpgImpl.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "RtpgImpl.h"
#include "TpgFaultMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "sa/RtpgStats.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID_SA

Rtpg*
new_Rtpg()
{
  return new RtpgImpl();
}


//////////////////////////////////////////////////////////////////////
// クラス RtpgImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
RtpgImpl::RtpgImpl()
{
}

// @brief デストラクタ
RtpgImpl::~RtpgImpl()
{
}

// @brief 乱数生成器を初期化する．
// @param[in] seed 乱数の種
void
RtpgImpl::init(ymuint32 seed)
{
  mRandGen.init(seed);
}

// @brief RTPGを行なう．
// @param[in] fmgr 故障マネージャ
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fsim 故障シミュレータ
// @param[in] min_f 1回のシミュレーションで検出する故障数の下限
// @param[in] max_i 故障検出できないシミュレーション回数の上限
// @param[in] max_pat 最大のパタン数
// @param[out] det_fault_list 検出された故障のリスト
// @param[out] tvlist テストベクタのリスト
// @param[out] stats 実行結果の情報を格納する変数
void
RtpgImpl::run(TpgFaultMgr& fmgr,
	      TvMgr& tvmgr,
	      Fsim& fsim,
	      int min_f,
	      int max_i,
	      int max_pat,
	      vector<const TestVector*>& tvlist,
	      RtpgStats& stats)
{
  StopWatch local_timer;

  local_timer.start();

  int fnum = 0;
  int undet_i = 0;
  int epat_num = 0;
  int total_det_count = 0;

  fsim.set_skip_all();
  for (int i = 0; i < fmgr.max_fault_id(); ++ i) {
    const TpgFault* f = fmgr.fault(i);
    if ( f != nullptr && fmgr.status(f) == kFsUndetected ) {
      fsim.clear_skip(f);
      ++ fnum;
    }
  }

  TestVector* tv_array[PV_BITLEN];
  for (int i = 0; i < PV_BITLEN; ++ i) {
    tv_array[i] = tvmgr.new_vector();
  }

  fsim.clear_patterns();
  int pat_num = 0;
  int wpos = 0;
  for ( ; ; ) {
    if ( pat_num < max_pat ) {
      TestVector* tv = tv_array[wpos];
      tv->set_from_random(mRandGen);
      fsim.set_pattern(wpos, tv);
      ++ pat_num;
      ++ wpos;
      if ( wpos < PV_BITLEN ) {
	continue;
      }
    }
    else if ( wpos == 0 ) {
      break;
    }

    int det_count = fsim.ppsfp();

    bool det_flags[PV_BITLEN];
    for (int i = 0; i < PV_BITLEN; ++ i) {
      det_flags[i] = false;
    }
    int num = wpos;
    for (int i = 0; i < det_count; ++ i) {
      const TpgFault* f = fsim.det_fault(i);
      fmgr.set_status(f, kFsDetected);
      fsim.set_skip(f);
      PackedVal dpat = fsim.det_fault_pat(i);
      // dpat の最初の1のビットを調べる．
      int first = 0;
      for ( ; first < num; ++ first) {
	if ( dpat & (1ULL << first) ) {
	  break;
	}
      }
      ASSERT_COND( first < num );
      det_flags[first] = true;
    }
    for (int i = 0; i < num; ++ i) {
      if ( det_flags[i] ) {
	// 検出できたパタンは tvlist に移す．
	const TestVector* tv = tv_array[i];
	tvlist.push_back(tv);
	// tv_array には新しいパタンを補充しておく．
	tv_array[i] = tvmgr.new_vector();
	++ epat_num;
      }
    }
    fsim.clear_patterns();
    wpos = 0;

    total_det_count += det_count;

    if ( total_det_count == fnum ) {
      // すべての故障を検出した．
      break;
    }
    if ( det_count < min_f ) {
      // 検出故障数の下限を下回った
      break;
    }
    if ( det_count > 0 ) {
      undet_i = 0;
    }
    else {
      ++ undet_i;
      if ( undet_i > max_i ) {
	// 未検出の回数が max_i を越えた．
	break;
      }
    }
  }

  for (int i = 0; i < PV_BITLEN; ++ i) {
    tvmgr.delete_vector(tv_array[i]);
  }

  local_timer.stop();
  USTime time = local_timer.time();

  stats.set(total_det_count, pat_num, epat_num, time);
}

END_NAMESPACE_DRUID_SA
