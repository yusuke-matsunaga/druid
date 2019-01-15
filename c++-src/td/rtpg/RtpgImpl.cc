
/// @file RtpgImpl.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "RtpgImpl.h"
#include "td/TvMgr.h"
#include "td/TestVector.h"
#include "td/Fsim.h"
#include "td/RtpgStats.h"
#include "FopRtpg.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID_TD

Rtpg*
new_Rtpg1()
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
// @param[in] fault_list 故障リスト
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fsim 故障シミュレータ
// @param[in] min_f 1回のシミュレーションで検出する故障数の下限
// @param[in] max_i 故障検出できないシミュレーション回数の上限
// @param[in] max_pat 最大のパタン数
// @param[out] det_fault_list 検出された故障のリスト
// @param[out] tvlist テストベクタのリスト
// @param[out] stats 実行結果の情報を格納する変数
void
RtpgImpl::run(const vector<const TpgFault*>& fault_list,
	       Tv2Mgr& tvmgr,
	       FsimT& fsim,
	       ymuint min_f,
	       ymuint max_i,
	       ymuint max_pat,
	       vector<const TpgFault*>& det_fault_list,
	       vector<TestVector2*>& tvlist,
	       RtpgStats& stats)
{
  StopWatch local_timer;

  local_timer.start();

  ymuint fnum = fault_list.size();
  ymuint undet_i = 0;
  ymuint epat_num = 0;
  ymuint total_det_count = 0;

  TestVector2* tv = tvmgr.new_vector();

  FopRtpg op(fsim);

  fsim.set_faults(fault_list);
  op.init();

  ymuint pat_num = 0;
  for( ; pat_num < max_pat; ++ pat_num) {
    tv->set_from_random(mRandGen);

    op.clear_count();

    fsim.sppfp(tv, op);

    ymuint det_count = op.count(0);
    if ( det_count > 0 ) {
      tvlist.push_back(tv);
      tv = tvmgr.new_vector();
      ++ epat_num;
    }

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

  det_fault_list = op.fault_list();

  tvmgr.delete_vector(tv);

  local_timer.stop();
  USTime time = local_timer.time();

  stats.set(total_det_count, pat_num, epat_num, time);
}

END_NAMESPACE_DRUID_TD
