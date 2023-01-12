
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

#include "ym/HashMap.h"
#include "ym/MinCov.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
Dtpg2::Dtpg2(
  const string& sat_type,
  const string& sat_option,
  ostream* sat_outp,
  BackTracer& bt
) : mSatType{sat_type},
    mSatOption{sat_option},
    mSatOutP{sat_outp},
    mBackTracer{bt}
{
}

// @brief デストラクタ
Dtpg2::~Dtpg2()
{
}

// @brief テスト生成を行う．
void
Dtpg2::run(
  TvMgr& tvmgr,
  TpgFaultMgr& fmgr,
  Fsim& fsim,
  const TpgNetwork& network,
  bool use_xorsampling,
  double wsa_ratio,
  int scount_limit,
  vector<const TestVector*>& tv_list,
  DtpgStats& stats
)
{
  cout << "scount_limit = " << scount_limit << endl;
  int wsa_limit = 0;
  { // 順序回路としてランダムに動かした時の
    // 平均の信号遷移回数を得る．
    int count = 10000;
    int warmup = 100;
    bool weighted = false;

    auto i_vect = tvmgr.new_input_vector();
    auto f_vect = tvmgr.new_dff_vector();
    double total_wsa = 0.0;
    RandGen rg;

    // 初期状態
    i_vect->set_from_random(rg);
    f_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *f_vect);

    for (int i = 0; i < warmup; ++ i) {
      // このシミュレーション結果は捨てる．
      // 状態を遷移させることが目的
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, weighted);
    }
    for (int i = 0; i < count; ++ i) {
      i_vect->set_from_random(rg);
      int wsa1 = fsim.calc_wsa(*i_vect, weighted);
      total_wsa += wsa1;
    }
    double ave_wsa = total_wsa / count;

    tvmgr.delete_vector(i_vect);
    tvmgr.delete_vector(f_vect);

    wsa_limit = static_cast<int>(ave_wsa * wsa_ratio);
  }

#if 0
  make_input_constraint(tvmgr, fsim, wsa_limit);
#endif

  mPatNum = 0;
  mExceedNum = 0;
  mTotalCount = 0;
  mTotalFound = 0;
  mTotalSampling = 0;
  mTotalOver = 0;
  mFinalExceedNum = 0;

  vector<TestVector*> initial_tv_list;
  vector<const TpgFault*> fault_list;
  HashMap<int, int> fault_map;
  TestVector* tv = tvmgr.new_vector();
  int nf = network.rep_fault_num();
  for (int i = 0; i < nf; ++ i) {
    auto fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      auto stat = dtpg(tvmgr, fsim, network, fault, use_xorsampling,
		       wsa_limit, scount_limit,
		       initial_tv_list, stats);
      if ( stat == kB3True ) {
	++ mPatNum;
	fmgr.set_status(fault, kFsDetected);

	int fpos = fault_list.size();
	fault_list.push_back(fault);
	fault_map.add(fault->id(), fpos);
      }
      else if ( stat == kB3False ) {
	fmgr.set_status(fault, kFsUntestable);
      }
    }
  }
  {
    double ave_loop = static_cast<double>(mTotalCount) / static_cast<double>(mExceedNum);
    double ave_hit = static_cast<double>(mTotalFound) / static_cast<double>(mTotalSampling);
    double ave_sample = static_cast<double>(mTotalOver) / static_cast<double>(mExceedNum);
    cout << "Total Detected Faults: " << mPatNum << endl
	 << "WSA limit:             " << wsa_limit << endl
	 << "Initial Exceed Pats:   " << mExceedNum << endl
	 << "Final Exceed Pats:     " << mFinalExceedNum << endl
	 << "Ave. loop counts:      " << ave_loop << endl
	 << "Ave. hit counts:       " << ave_hit << endl
	 << "Ave. samples:          " << ave_sample << endl;
  }

  {
    int np = initial_tv_list.size();
    int nf = fault_list.size();

    MinCov mc;
    mc.set_size(nf, np);

    Fsim* fsim = Fsim::new_Fsim2(network, FaultType::TransitionDelay);
    for (int i = 0; i < np; ++ i) {
      auto tv = initial_tv_list[i];
      fsim->clear_skip_all();
      int nd = fsim->sppfp(tv);
      for (int j = 0; j < nd; ++ j) {
	auto f = fsim->det_fault(j);
	int k;
	bool stat = fault_map.find(f->id(), k);
	ASSERT_COND( stat );
	mc.insert_elem(k, i);
      }
    }
    cout << "initial patterns: " << np << endl;
    vector<int> solution;
    int c = mc.heuristic(solution);
    cout << "optimized patterns: " << solution.size() << endl;

    tv_list.clear();
    tv_list.resize(solution.size());
    for (int i = 0; i < solution.size(); ++ i) {
      const TestVector* tv = initial_tv_list[solution[i]];
      tv_list[i] = tv;
    }
    delete fsim;
  }
}

// @brief テスト生成を行なう．
SatBool3
Dtpg2::dtpg(
  TvMgr& tvmgr,
  Fsim& fsim,
  const TpgNetwork& network,
  const TpgFault* fault,
  bool use_xorsampling,
  int wsa_limit,
  int scount_limit,
  vector<TestVector*>& tv_list,
  DtpgStats& stats
)
{
  Dtpg2Impl impl{mSatType, mSatOption, mSatOutP, mBackTracer,
		 network, fault->ffr()->root()};
  impl.gen_cnf(stats);

  // 今の故障に関係のある PPI の数を数える．
  int xor_num = impl.make_xor_list();
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

  int count_limit = 50;
  int fcount_limit = 20;
  int count = 0;
  int fcount = 0;
  int scount = 0;

  NodeValList nodeval_list;
  SatBool3 ans = impl.dtpg(fault, nodeval_list, stats);
  if ( ans != kB3True ) {
    return ans;
  }

  auto tv = tvmgr.new_vector();
  int wsa = optimize(tvmgr, fsim, wsa_limit, nodeval_list, tv);
  if ( wsa <= wsa_limit ) {
    tv_list.push_back(tv);
    tv = tvmgr.new_vector();
    ++ scount;
    if ( scount >= scount_limit ) {
      goto exit;
    }
  }

  if ( !use_xorsampling ) {
    if ( wsa > wsa_limit ) {
      // 違反しているけど解に入れる．
      tv_list.push_back(tv);
    }
    return kB3True;
  }

  {
    bool exit = false;
    auto tv_min = tvmgr.new_vector();
    int wsa_min = UINT_MAX;
    for ( ; count < count_limit; ++ count) {
      ++ mTotalCount;
      Dtpg2Impl impl2{mSatType, mSatOption, mSatOutP, mBackTracer,
		      network, fault->ffr()->root()};
      impl2.gen_cnf(stats);
      impl2.make_xor_list();
      impl2.add_xor_constraint(xor_num, mRandGen);

      int xn_exp = 1U << xor_num;
      for (int p = 0U; p < xn_exp; ++ p) {
	++ mTotalSampling;
	NodeValList nodeval_list1;
	auto ans = impl2.dtpg_with_xor(fault, p, nodeval_list1, stats);
	if ( ans != kB3True ) {
	  continue;
	}

	++ mTotalFound;

	wsa = optimize(tvmgr, fsim, wsa_limit, nodeval_list1, tv);
	if ( wsa <= wsa_limit ) {
	  tv_list.push_back(tv);
	  tv = tvmgr.new_vector();

	  ++ scount;
	  if ( scount >= scount_limit ) {
	    exit = true;
	    break;
	  }
	}
	else {
	  if ( wsa_min > wsa ) {
	    wsa_min = wsa;
	    tv_min->copy(*tv);
	  }

	  ++ mTotalOver;

	  ++ fcount;
	  if ( fcount > fcount_limit ) {
	    exit = true;
	    break;
	  }
	}
      }
      if ( exit ) {
	break;
      }
    }

    if ( wsa > wsa_limit ) {
      ++ mFinalExceedNum;
      tv_list.push_back(tv_min);
    }
    else {
      tvmgr.delete_vector(tv_min);
    }
  }

 exit:
  tvmgr.delete_vector(tv);

  return kB3True;
}

int
Dtpg2::optimize(
  TvMgr& tvmgr,
  Fsim& fsim,
  int wsa_limit,
  const NodeValList& nodeval_list,
  TestVector* tv
)
{
  int ni = tvmgr.input_num();
  int nd = tvmgr.dff_num();
  int nall = ni + ni + nd;
  int n = nodeval_list.size();
  int nx = nall - n;
  vector<int> x_list;
  { // nodeval_list に現れない入力の番号を x_list に入れる．
    vector<bool> i0_map(ni, true);
    vector<bool> i1_map(ni, true);
    vector<bool> d0_map(nd, true);
    for (int i = 0; i < n; ++ i) {
      auto nv = nodeval_list[i];
      const TpgNode* node = nv.node();
      if ( node->is_primary_input() ) {
	int id = node->input_id();
	if ( nv.time() == 1 ) {
	  i1_map[id] = false;
	}
	else {
	  i0_map[id] = false;
	}
      }
      else if ( node->is_dff_output() ) {
	ASSERT_COND( nv.time() == 0 );
	int id = node->dff()->id();
	d0_map[id] = false;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
    x_list.reserve(nx);
    for (int i = 0; i < ni; ++ i) {
      if ( i0_map[i] ) {
	x_list.push_back(i);
      }
    }
    for (int i = 0; i < nd; ++ i) {
      if ( d0_map[i] ) {
	x_list.push_back(i + ni);
      }
    }
    for (int i = 0; i < ni; ++ i) {
      if ( i1_map[i] ) {
	x_list.push_back(i + ni + nd);
      }
    }
    ASSERT_COND( x_list.size() == nx );
  }

  //int count_limit = nx * 1;
  int count_limit = 100;

  tv->init();
  tv->set_from_assign_list(nodeval_list);
  tv->fix_x_from_random(mRandGen2);
  int wsa = fsim.calc_wsa(tv, false);
  TestVector* tv1 = tvmgr.new_vector();
  for (double t = 100; wsa > wsa_limit && t > 0.001; t *= 0.6) {
    for (int count = 0; wsa > wsa_limit && count < count_limit; ++ count) {
      // tv の一つのビットをランダムに反転させる．
      tv1->copy(*tv);
      int pos = mRandGen2.int32() % nx;
      int id = x_list[pos];
      if ( id < ni ) {
	// 0時刻の入力
	Val3 val = tv1->input_val(id);
	tv1->set_input_val(id, ~val);
      }
      else if ( id < (ni + nd) ) {
	// 0時刻のフリップフロップ
	id -= ni;
	Val3 val = tv1->dff_val(id);
	tv1->set_dff_val(id, ~val);
      }
      else {
	// 1時刻の入力
	id -= (ni + nd);
	Val3 val = tv1->aux_input_val(id);
	tv1->set_aux_input_val(id, ~val);
      }
      int wsa1 = fsim.calc_wsa(tv1, false);
      if ( wsa1 <= wsa ) {
	tv->copy(*tv1);
	wsa = wsa1;
	if ( wsa1 <= wsa_limit ) {
	  break;
	}
      }
      else {
	double r = (wsa - wsa1) / (static_cast<double>(wsa) * t);
	double p = exp(r);
	double th = mRandGen2.real1();
	if ( p < th ) {
	  tv->copy(*tv1);
	}
      }
    }
  }
  tvmgr.delete_vector(tv1);

  return wsa;
}

void
Dtpg2::rtpg(TvMgr& tvmgr,
	    TpgFaultMgr& fmgr,
	    Fsim& fsim,
	    int wsa_limit,
	    DetectOp& dop)
{
  int max_pat = 10000;
  int min_f = 0;
  int max_i = 4;

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
      for ( ; ; ) {
	tv->set_from_random(mRandGen);
	int wsa = fsim.calc_wsa(tv, false);
	if ( wsa <= wsa_limit ) {
	  break;
	}
      }
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

    const TpgFault* det_flags[PV_BITLEN];
    for (int i = 0; i < PV_BITLEN; ++ i) {
      det_flags[i] = nullptr;
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
      det_flags[first] = f;
    }
    for (int i = 0; i < num; ++ i) {
      const TpgFault* f = det_flags[i];
      if ( f != nullptr ) {
	// 検出できたパタンは tvlist に移す．
	const TestVector* tv = tv_array[i];
	dop(f, tv);
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
}

Val3
read_val(const TestVector* tv,
	 int pos)
{
  int ni = tv->input_num();
  int nd = tv->dff_num();
  if ( pos < ni ) {
    return tv->input_val(pos);
  }
  pos -= ni;
  if ( pos < nd ) {
    return tv->dff_val(pos);
  }
  pos -= nd;
  return tv->aux_input_val(pos);
}

void
Dtpg2::make_input_constraint(TvMgr& tvmgr,
			     Fsim& fsim,
			     int wsa_limit)
{
  int count_limit = 10000;
  int warm_up = 10;
  RandGen rg;
  vector<TestVector*> on_list;
  vector<TestVector*> off_list;
  for (int i = 0; i < count_limit; ++ i) {
    TestVector* tv = tvmgr.new_vector();
    tv->set_from_random(rg);
    int wsa1 = fsim.calc_wsa(tv, false);
    if ( wsa1 <= wsa_limit ) {
      on_list.push_back(tv);
    }
    else {
      off_list.push_back(tv);
    }
  }
  InputVector* i_vect = tvmgr.new_input_vector();
  DffVector* d_vect = tvmgr.new_dff_vector();
  InputVector* a_vect = tvmgr.new_input_vector();
  for (int i = 0; i < count_limit; ++ i) {
    i_vect->set_from_random(rg);
    d_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *d_vect);
    for (int j = 0; j < warm_up; ++ j) {
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, false);
    }
    fsim.get_state(*i_vect, *d_vect);
    a_vect->set_from_random(rg);
    int wsa1 = fsim.calc_wsa(*i_vect, false);
    TestVector* tv = tvmgr.new_vector();
    tv->set(*i_vect, *d_vect, *a_vect);
    if ( wsa1 <= wsa_limit ) {
      on_list.push_back(tv);
    }
    else {
      off_list.push_back(tv);
    }
  }
  tvmgr.delete_vector(i_vect);
  tvmgr.delete_vector(d_vect);
  tvmgr.delete_vector(a_vect);

  {
    cout << "wsa_limit = " << wsa_limit << endl
	 << " ON patterns:  " << on_list.size() << endl
	 << " OFF patterns: " << off_list.size() << endl;
  }

  int ni = tvmgr.input_num();
  int nd = tvmgr.dff_num();
  int nall = ni + ni + nd;
  for (int i1 = 0; i1 < nall; ++ i1) {
    for (int i2 = i1 + 1; i2 < nall; ++ i2) {
      for (int i3 = i2 + 1; i3 < nall; ++ i3) {
	int n000 = 0;
	int n001 = 0;
	int n010 = 0;
	int n011 = 0;
	int n100 = 0;
	int n101 = 0;
	int n110 = 0;
	int n111 = 0;
	for (int i = 0; i < on_list.size(); ++ i) {
	  TestVector* tv = on_list[i];
	  Val3 v1 = read_val(tv, i1);
	  Val3 v2 = read_val(tv, i2);
	  Val3 v3 = read_val(tv, i3);
	  if ( v1 == kVal0 ) {
	    if ( v2 == kVal0 ) {
	      if ( v3 == kVal0 ) {
		++ n000;
	      }
	      else {
		++ n001;
	      }
	    }
	    else {
	      if ( v3 == kVal0 ) {
		++ n010;
	      }
	      else {
		++ n011;
	      }
	    }
	  }
	  else {
	    if ( v2 == kVal0 ) {
	      if ( v3 == kVal0 ) {
		++ n100;
	      }
	      else {
		++ n101;
	      }
	    }
	    else {
	      if ( v3 == kVal0 ) {
		++ n110;
	      }
	      else {
		++ n111;
	      }
	    }
	  }
	}
	if ( n000 == 0 ) {
	  cout << " ~" << i1 << ": ~" << i2 << ": ~" << i3 << endl;
	}
	if ( n001 == 0 ) {
	  cout << " ~" << i1 << ": ~" << i2 << ":  " << i3 << endl;
	}
	if ( n010 == 0 ) {
	  cout << " ~" << i1 << ":  " << i2 << ": ~" << i3 << endl;
	}
	if ( n011 == 0 ) {
	  cout << " ~" << i1 << ":  " << i2 << ":  " << i3 << endl;
	}
	if ( n100 == 0 ) {
	  cout << "  " << i1 << ": ~" << i2 << ": ~" << i3 << endl;
	}
	if ( n101 == 0 ) {
	  cout << "  " << i1 << ": ~" << i2 << ":  " << i3 << endl;
	}
	if ( n110 == 0 ) {
	  cout << "  " << i1 << ":  " << i2 << ": ~" << i3 << endl;
	}
	if ( n111 == 0 ) {
	  cout << "  " << i1 << ":  " << i2 << ":  " << i3 << endl;
	}
      }
    }
  }


  for (int i = 0; i < on_list.size(); ++ i) {
    tvmgr.delete_vector(on_list[i]);
  }
  for (int i = 0; i < off_list.size(); ++ i) {
    tvmgr.delete_vector(off_list[i]);
  }
}

END_NAMESPACE_DRUID
