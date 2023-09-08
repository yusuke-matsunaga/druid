
/// @file RtpgP2.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "RtpgP2.h"
#include "TpgFaultMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "td/RtpgStats.h"
#include "ym/StopWatch.h"
#include "ym/RandCombiGen.h"


BEGIN_NAMESPACE_DRUID_TD

Rtpg*
new_RtpgP2(ymuint nbits)
{
  RtpgP2* rtpg = new RtpgP2(nbits);
  return rtpg;
}

double
evaluate(ymuint sa,
	 ymuint thval)
{
  double thf = static_cast<double>(thval);
  double x = static_cast<double>(sa) - thf;
#if 0
  x /= thf;
  return 1.0 / (exp(x) + 1);
#else
#if 0
  return exp(- static_cast<double>(sa) / static_cast<double>(thval));
#else
  if ( sa < thval ) {
    return 1.0;
  }
  else {
    return 1.0 - (x / thval);
  }
#endif
#endif
}

void
gen_neighbor(const TestVector* tv1,
	     TestVector* tv2,
	     RandGen& randgen,
	     ymuint nbits)
{
  tv2->copy(*tv1);

  ymuint count = 0;
  for (ymuint i = 0; i < nbits; ++ i) {
    double r = randgen.real1();
    if ( r < 0.8 ) {
      ++ count;
    }
  }
  // count ビットだけ反転する．
  RandCombiGen rcg(tv1->input_num(), count);
  rcg.generate(randgen);
  for (ymuint i = 0; i < nbits; ++ i) {
    ymuint pos = rcg.elem(i);
    Val3 val = tv2->aux_input_val(pos);
    tv2->set_aux_input_val(pos, ~val);
  }
#warning "TODO: dff も考慮する．"
}


//////////////////////////////////////////////////////////////////////
// クラス RtpgP2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] nbits 変更するビット数
RtpgP2::RtpgP2(ymuint nbits) :
  mNbits(nbits)
{
}

// @brief デストラクタ
RtpgP2::~RtpgP2()
{
}

// @brief 乱数生成器を初期化する．
// @param[in] seed 乱数の種
void
RtpgP2::init(std::uint32_t seed)
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
// @param[out] tvlist テストベクタのリスト
// @param[out] stats 実行結果の情報を格納する変数
void
RtpgP2::run(TpgFaultMgr& fmgr,
	    TvMgr& tvmgr,
	    Fsim& fsim,
	    ymuint min_f,
	    ymuint max_i,
	    ymuint max_pat,
	    ymuint wsa_limit,
	    vector<const TestVector*>& tvlist,
	    RtpgStats& stats)
{
  StopWatch local_timer;

  local_timer.start();

  ymuint fnum = 0;
  ymuint undet_i = 0;
  ymuint epat_num = 0;
  ymuint total_det_count = 0;

  fsim.set_skip_all();
  for (ymuint i = 0; i < fmgr.max_fault_id(); ++ i) {
    const TpgFault* f = fmgr.fault(i);
    if ( fmgr.status(f) == kFsUndetected ) {
      fsim.clear_skip(f);
      ++ fnum;
    }
  }

  TestVector* tv1 = tvmgr.new_vector();
  TestVector* tv2 = tvmgr.new_vector();

  ymuint gnum = 0;
  ymuint pat_num = 0;

  tv1->set_from_random(mRandGen);
#if 0
  ymuint wsa0 = fsim.calc_wsa(tv1);
#else
  ymuint wsa0 = 0;
#endif
  double val0 = evaluate(wsa0, wsa_limit);
  ymuint count = 0;

  while ( pat_num < max_pat ) {
    // tv1 から tv2 を作る．
    gen_neighbor(tv1, tv2, mRandGen, mNbits);
    ++ gnum;
#if 0
    ymuint wsa1 = fsim.calc_wsa(tv2);
#else
    ymuint wsa1 = 0;
#endif
    double val1 = evaluate(wsa1, wsa_limit);
    //cout << "wsa = " << wsa1 << ", val = " << val1
    //<< ", # of pat = " << pat_num << endl;

    bool accept = false;
    if ( val1 >= val0 ) {
      accept = true;
    }
    else {
      double r = mRandGen.real1();
      double ratio = val1 / val0;
      if ( r < ratio ) {
	accept = true;
      }
    }
    if ( accept ) {
      tv1->copy(*tv2);
      wsa0 = wsa1;
      val0 = val1;


      if ( wsa1 <= wsa_limit ) {

	ymuint det_count = fsim.sppfp(tv1);
	++ pat_num;

	if ( det_count > 0 ) {
	  tvlist.push_back(tv1);
	  tv1 = tvmgr.new_vector();
	  ++ epat_num;
	  for (ymuint i = 0; i < det_count; ++ i) {
	    const TpgFault* f = fsim.det_fault(i);
	    fmgr.set_status(f, kFsDetected);
	    fsim.set_skip(f);
	  }
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
    }
  }

  tvmgr.delete_vector(tv1);
  tvmgr.delete_vector(tv2);

  local_timer.stop();
  USTime time = local_timer.time();

  stats.set(total_det_count, pat_num, epat_num, time);

  cout << "# of generated patterns = " << gnum << endl;
}

END_NAMESPACE_DRUID_TD
