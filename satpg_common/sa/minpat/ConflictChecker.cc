﻿
/// @file ConflictChecker.cc
/// @brief ConflictChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "ConflictChecker.h"

#include "FaultAnalyzer.h"

#include "TpgFault.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"

#include "sa/StructSat.h"
#include "sa/FoCone.h"

#include "ym/RandGen.h"
#include "ym/StopWatch.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG_SA

BEGIN_NONAMESPACE

// 2つのリストが共通要素を持つとき true を返す．
// リストはソートされていると仮定する．
inline
bool
check_intersect(const vector<ymuint>& list1,
		const vector<ymuint>& list2)
{
  ymuint n1 = list1.size();
  ymuint n2 = list2.size();
  ymuint i1 = 0;
  ymuint i2 = 0;
  ymuint v1 = list1[i1];
  ymuint v2 = list2[i2];
  for ( ; ; ) {
    if ( v1 < v2 ) {
      ++ i1;
      if ( i1 >= n1 ) {
	return false;
      }
      v1 = list1[i1];
    }
    else if ( v1 > v2 ) {
      ++ i2;
      if ( i2 >= n2 ) {
	return false;
      }
      v2 = list2[i2];
    }
    else {
      return true;
    }
  }
  return false;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス ConflictChecker
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ConflictChecker::ConflictChecker(FaultAnalyzer& analyzer,
				 TvMgr& tvmgr,
				 Fsim& fsim) :
  mAnalyzer(analyzer),
  mTvMgr(tvmgr),
  mFsim(fsim)
{
  mVerbose = mAnalyzer.verbose();
  mMaxNodeId = mAnalyzer.max_node_id();
  mMaxFaultId = mAnalyzer.max_fault_id();
  mFaultDataArray.resize(mMaxFaultId);
  for (ymuint i = 0; i < mMaxFaultId; ++ i) {
    mFaultDataArray[i].mDetCount = 0;
  }
}

// @brief デストラクタ
ConflictChecker::~ConflictChecker()
{
}

// @brief verbose フラグを設定する．
// @param[in] verbose 表示を制御するフラグ
void
ConflictChecker::set_verbose(int verbose)
{
  mVerbose = verbose;
}

// @brief 故障間の衝突性を調べる．
void
ConflictChecker::analyze_conflict(const vector<ymuint>& fid_list)
{
  StopWatch local_timer;
  local_timer.start();

  mConflictStats.conf_timer.reset();
  mConflictStats.conf1_timer.reset();
  mConflictStats.conf2_timer.reset();
  mConflictStats.conf3_timer.reset();
  mConflictStats.conf4_timer.reset();
  mConflictStats.int1_timer.reset();
  mConflictStats.int2_timer.reset();
  mConflictStats.conf_count = 0;
  mConflictStats.conf1_count = 0;
  mConflictStats.conf2_count = 0;
  mConflictStats.conf3_count = 0;
  mConflictStats.conf4_count = 0;
  mConflictStats.conf4_check_count = 0;
  mConflictStats.int1_count = 0;
  mConflictStats.int2_count = 0;

  // シミュレーション結果を用いてコンフリクトチェックのスクリーニングを行う．
  do_fsim(fid_list);

  ymuint fault_num = fid_list.size();
  for (ymuint i1 = 0; i1 < fault_num; ++ i1) {
    ymuint f1_id = fid_list[i1];

    if ( mVerbose > 1 ) {
      cout << "\rCFL: " << setw(6) << i1 << " / " << setw(6) << fault_num;
      cout.flush();
    }

    const vector<ymuint>& ma_conf_list = mFaultDataArray[f1_id].mMaConflictList;
    for (ymuint i = 0; i < ma_conf_list.size(); ++ i) {
      ymuint f2_id = ma_conf_list[i];
      mFaultDataArray[f1_id].mConflictList.push_back(f2_id);
    }

    vector<ymuint>& f2_list = mFaultDataArray[f1_id].mCandList;
    vector<ymuint> conf_list;
    analyze_conflict(f1_id, f2_list, conf_list, false, false);
    for (ymuint i = 0; i < conf_list.size(); ++ i) {
      ymuint f2_id = conf_list[i];
      mFaultDataArray[f1_id].mConflictList.push_back(f2_id);
      mFaultDataArray[f2_id].mConflictList.push_back(f1_id);
    }
  }

  for (ymuint i1 = 0; i1 < fault_num; ++ i1) {
    ymuint f1_id = fid_list[i1];
    FaultData& fd = mFaultDataArray[f1_id];
    sort(fd.mConflictList.begin(), fd.mConflictList.end());
  }

  local_timer.stop();

  if ( mVerbose ) {
    cout << endl;
    print_conflict_stats(cout);
    cout << "Total CPU time " << local_timer.time() << endl;
  }
}

// @brief 衝突リストを得る．
const vector<ymuint>&
ConflictChecker::conflict_list(ymuint fid)
{
  ASSERT_COND( fid < mMaxFaultId );
  return mFaultDataArray[fid].mConflictList;
}

// @brief 1つの故障に対する衝突の解析を行う．
void
ConflictChecker::analyze_conflict(ymuint f1_id,
				  const vector<ymuint>& fid_list,
				  vector<ymuint>& conf_list)
{
  const vector<ymuint>& ma_conf_list = mFaultDataArray[f1_id].mMaConflictList;
  vector<ymuint>& f2_list = mFaultDataArray[f1_id].mCandList;
  vector<ymuint> conf1_list;
  analyze_conflict(f1_id, f2_list, conf1_list, false, false);

  conf_list.clear();
  conf_list.reserve(ma_conf_list.size() + conf1_list.size());
  for (ymuint i = 0; i < ma_conf_list.size(); ++ i) {
    ymuint f2_id = ma_conf_list[i];
    conf_list.push_back(f2_id);
  }
  for (ymuint i = 0; i < conf1_list.size(); ++ i) {
    ymuint f2_id = conf1_list[i];
    conf_list.push_back(f2_id);
  }
}

// @brief 衝突数の見積もりを行う．
void
ConflictChecker::estimate_conflict(const vector<ymuint>& fid_list,
				   vector<ymuint>& conf_num_array)
{
  conf_num_array.clear();
  conf_num_array.resize(mMaxFaultId, 0);
  ymuint fault_num = fid_list.size();
  for (ymuint i1 = 0; i1 < fault_num; ++ i1) {
    ymuint f1_id = fid_list[i1];

    const vector<ymuint>& ma_conf_list = mFaultDataArray[f1_id].mMaConflictList;
    vector<ymuint>& f2_list = mFaultDataArray[f1_id].mCandList;
    conf_num_array[i1] = ma_conf_list.size() + f2_list.size();
  }
}

// @brief 1つの故障と複数の故障間の衝突性を調べる．
void
ConflictChecker::analyze_conflict(ymuint f1_id,
				  const vector<ymuint>& f2_list,
				  vector<ymuint>& conf_list,
				  bool simple,
				  bool local_verbose)
{
  mConflictStats.conf_timer.start();

  const FaultInfo& fi1 = mAnalyzer.fault_info(f1_id);
  const NodeValList& suf_list1 = fi1.sufficient_assignment();
  const NodeValList& ma_list1 = fi1.mandatory_assignment();

#if 0
  GvalCnf gval_cnf(mMaxNodeId, string(), string(), nullptr);

  // f1 を検出する CNF を生成
  gval_cnf.add_assignments(ma_list1);
  if ( !fi1.single_cube() ) {
    FvalCnf fval_cnf(gval_cnf);
    const TpgFault* f1 = mAnalyzer.fault(f1_id);
    NodeSet node_set1;
    node_set1.mark_region(mMaxNodeId, f1->tpg_onode());
    fval_cnf.make_cnf(f1, node_set1, kVal1);
  }

  conf_list.reserve(f2_list.size());
  for (ymuint i2 = 0; i2 < f2_list.size(); ++ i2) {
    ymuint f2_id = f2_list[i2];

    if ( f1_id > f2_id ) {
      continue;
    }

    const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
    const NodeValList& suf_list2 = fi2.sufficient_assignment();
    const NodeValList& ma_list2 = fi2.mandatory_assignment();

    mConflictStats.int2_timer.start();
    SatBool3 sat_stat = gval_cnf.check_sat(suf_list2);
    if ( sat_stat == kB3True ) {
      // f2 の十分割当のもとで f1 が検出できれば f1 と f2 はコンフリクトしない．
      ++ mConflictStats.int2_count;
      mConflictStats.int2_timer.stop();
      continue;
    }
    mConflictStats.int2_timer.stop();

    if ( fi2.single_cube() ) {
      if ( sat_stat == kB3False ) {
	++ mConflictStats.conf_count;
	++ mConflictStats.conf3_count;
	conf_list.push_back(f2_id);
      }
      // f2 の十分割当と必要割当が等しければ上のチェックで終わり．
      continue;
    }

    mConflictStats.conf3_timer.start();
    if ( gval_cnf.check_sat(ma_list2) == kB3False ) {
      // f2 の必要割当のもとで f1 が検出できなければ f1 と f2 はコンフリクトしている．
      ++ mConflictStats.conf_count;
      ++ mConflictStats.conf3_count;
      conf_list.push_back(f2_id);
      mConflictStats.conf3_timer.stop();
      continue;
    }
    mConflictStats.conf3_timer.stop();

    if ( simple ) {
      continue;
    }

    mConflictStats.conf4_timer.start();
    ++ mConflictStats.conf4_check_count;
    {
      GvalCnf gval_cnf(mMaxNodeId, string(), string(), nullptr);

      // f1 を検出する CNF を生成
      const FaultInfo& fi1 = mAnalyzer.fault_info(f1_id);
      gval_cnf.add_assignments(fi1.mandatory_assignment());
      if ( !fi1.single_cube() ) {
	FvalCnf fval_cnf1(gval_cnf);
	const TpgFault* f1 = mAnalyzer.fault(f1_id);
	NodeSet node_set1;
	node_set1.mark_region(mMaxNodeId, f1->tpg_onode());
	fval_cnf1.make_cnf(f1, node_set1, kVal1);
      }

      // f2 を検出する CNF を生成
      const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
      gval_cnf.add_assignments(fi2.mandatory_assignment());
      FvalCnf fval_cnf2(gval_cnf);
      const TpgFault* f2 = mAnalyzer.fault(f2_id);
      NodeSet node_set2;
      node_set2.mark_region(mMaxNodeId, f2->tpg_onode());
      fval_cnf2.make_cnf(f2, node_set2, kVal1);

      SatBool3 sat_stat = gval_cnf.check_sat();
      if ( sat_stat == kB3False ) {
	++ mConflictStats.conf_count;
	++ mConflictStats.conf4_count;
	conf_list.push_back(f2_id);
      }
    }
    mConflictStats.conf4_timer.stop();
  }
#else
  StructSat struct_sat(mMaxNodeId);

  // f1 を検出する CNF を生成
  struct_sat.add_assignments(ma_list1);
  if ( !fi1.single_cube() ) {
    const TpgFault* f1 = mAnalyzer.fault(f1_id);
    struct_sat.add_focone(f1, kVal1);
  }

  conf_list.reserve(f2_list.size());
  for (ymuint i2 = 0; i2 < f2_list.size(); ++ i2) {
    ymuint f2_id = f2_list[i2];

    if ( f1_id > f2_id ) {
      continue;
    }

    const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
    const NodeValList& suf_list2 = fi2.sufficient_assignment();
    const NodeValList& ma_list2 = fi2.mandatory_assignment();

    mConflictStats.int2_timer.start();
    SatBool3 sat_stat = struct_sat.check_sat(suf_list2);
    if ( sat_stat == kB3True ) {
      // f2 の十分割当のもとで f1 が検出できれば f1 と f2 はコンフリクトしない．
      ++ mConflictStats.int2_count;
      mConflictStats.int2_timer.stop();
      continue;
    }
    mConflictStats.int2_timer.stop();

    if ( fi2.single_cube() ) {
      if ( sat_stat == kB3False ) {
	++ mConflictStats.conf_count;
	++ mConflictStats.conf3_count;
	conf_list.push_back(f2_id);
      }
      // f2 の十分割当と必要割当が等しければ上のチェックで終わり．
      continue;
    }

    mConflictStats.conf3_timer.start();
    if ( struct_sat.check_sat(ma_list2) == kB3False ) {
      // f2 の必要割当のもとで f1 が検出できなければ f1 と f2 はコンフリクトしている．
      ++ mConflictStats.conf_count;
      ++ mConflictStats.conf3_count;
      conf_list.push_back(f2_id);
      mConflictStats.conf3_timer.stop();
      continue;
    }
    mConflictStats.conf3_timer.stop();

    if ( simple ) {
      continue;
    }

    mConflictStats.conf4_timer.start();
    ++ mConflictStats.conf4_check_count;
    {
      StructSat struct_sat(mMaxNodeId);

      // f1 を検出する CNF を生成
      const FaultInfo& fi1 = mAnalyzer.fault_info(f1_id);
      struct_sat.add_assignments(fi1.mandatory_assignment());
      if ( !fi1.single_cube() ) {
	const TpgFault* f1 = mAnalyzer.fault(f1_id);
	struct_sat.add_focone(f1, kVal1);
      }

      // f2 を検出する CNF を生成
      const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
      struct_sat.add_assignments(fi2.mandatory_assignment());
      const TpgFault* f2 = mAnalyzer.fault(f2_id);
      struct_sat.add_focone(f2, kVal1);

      SatBool3 sat_stat = struct_sat.check_sat();
      if ( sat_stat == kB3False ) {
	++ mConflictStats.conf_count;
	++ mConflictStats.conf4_count;
	conf_list.push_back(f2_id);
      }
    }
    mConflictStats.conf4_timer.stop();
  }
#endif

  mConflictStats.conf_timer.stop();

  if ( mVerbose > 0 && local_verbose ) {
    if ( mVerbose > 1 ) {
      cout << endl;
    }
    print_conflict_stats(cout);
  }
}

// @brief analyze_conflict の統計情報を出力する．
void
ConflictChecker::print_conflict_stats(ostream& s)
{
  s << "Total    " << setw(6) << mConflictStats.conf_count  << " conflicts" << endl;
  s << "Total    " << setw(6) << mConflictStats.conf3_count << " conflicts (single ma_list)" << endl;
  s << "Total    " << setw(6) << mConflictStats.conf4_count << " conflicts (exact) / "
       << setw(6) << mConflictStats.conf4_check_count << endl;
  s << "Total    " << setw(6) << mConflictStats.int2_count  << " suf_list intersection check" << endl;
  s << "CPU time (conflict check)    " << mConflictStats.conf_timer.time() << endl;
  s << "CPU time (single conflict)   " << mConflictStats.conf3_timer.time() << endl;
  s << "CPU time (exact conflict)    " << mConflictStats.conf4_timer.time() << endl;
  s << "CPU time (single suf_list)   " << mConflictStats.int2_timer.time() << endl;
}

// @brief 故障シミュレーションを行い，故障検出パタンを記録する．
// @param[in] fid_list 故障リスト
void
ConflictChecker::do_fsim(const vector<ymuint>& fid_list)
{
  StopWatch local_timer;
  local_timer.start();

  mFsim.clear_patterns();
  ymuint nf = fid_list.size();
  ymuint wpos = 0;
  ymuint npat = fid_list.size();
  ymuint base = 0;
  for (ymuint i = 0; ; ++ i) {
    if ( i < nf ) {
      ymuint fid = fid_list[i];
      const FaultInfo& fi = mAnalyzer.fault_info(fid);
      TestVector* tv = fi.testvector();
      mFsim.set_pattern(wpos, tv);
      ++ wpos;
      if ( wpos < kPvBitLen ) {
	continue;
      }
    }
    else if ( wpos == 0 ) {
      break;
    }

    if ( mVerbose > 1 ) {
      cout << "\rFSIM: " << base;
      cout.flush();
    }
    mFsim.sa_ppsfp();
    record_pat(fid_list);
    base += wpos;
    mFsim.clear_patterns();
    wpos = 0;
  }

  TestVector* cur_array[kPvBitLen];
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    cur_array[i] = mTvMgr.new_sa_vector();
  }

  ymuint nochg_count = 0;
  for (ymuint c = 0; c < 1000; ++ c) {
    mFsim.clear_patterns();
    for (ymuint i = 0; i < kPvBitLen; ++ i) {
      cur_array[i]->set_from_random(mRandGen);
      mFsim.set_pattern(i, cur_array[i]);
    }

    if ( mVerbose > 1 ) {
      cout << "\rFSIM: " << base;
      cout.flush();
    }
    mFsim.sa_ppsfp();
    ymuint nchg = 0;
    nchg += record_pat(fid_list);
    base += kPvBitLen;
    if ( nchg == 0 ) {
      ++ nochg_count;
      if ( nochg_count > 3 ) {
	break;
      }
    }
    else {
      nochg_count = 0;
    }
  }
  if ( mVerbose > 1 ) {
    cout << endl;
  }

  // 乱数パタンは削除しておく．
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    mTvMgr.delete_vector(cur_array[i]);
  }

  // mCandList の後始末．
  for (ymuint i = 0; i < fid_list.size(); ++ i) {
    ymuint fid = fid_list[i];
    FaultData& fd = mFaultDataArray[fid];
    if ( fd.mCandList.size() != fd.mCandListSize ) {
      fd.mCandList.erase(fd.mCandList.begin() + fd.mCandListSize, fd.mCandList.end());
    }
  }

  local_timer.stop();

  if ( mVerbose ) {
    cout << "CPU time (fault simulation)  " << local_timer.time() << endl
	 << "Total " << base << " patterns simulated" << endl;
  }

}

ymuint
ConflictChecker::record_pat(const vector<ymuint>& fid_list)
{
  ymuint n = mFsim.det_fault_num();
  ymuint nchg = 0;
  vector<PackedVal> det_flag(mMaxFaultId, false);
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = mFsim.det_fault(i);
    PackedVal pv = mFsim.det_fault_pat(i);
    det_flag[f->id()] = pv;
  }

  // 検出結果を用いて支配される故障の候補リストを作る．
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f1 = mFsim.det_fault(i);
    ymuint f1_id = f1->id();
    PackedVal bv1 = mFsim.det_fault_pat(i);
    FaultData& fd1 = mFaultDataArray[f1_id];

    if ( fd1.mDetCount == 0 ) {
      // 初めて検出された場合
      // 構造的に独立でない故障を候補にする．
      const FaultInfo& fi1 = mAnalyzer.fault_info(f1_id);
      const NodeValList& pi_suf_list1 = fi1.pi_sufficient_assignment();
      const NodeValList& ma_list1 = fi1.mandatory_assignment();
      const vector<ymuint>& input_list1 = mAnalyzer.input_list(f1_id);
      for (ymuint j = 0; j < fid_list.size(); ++ j) {
	ymuint f2_id = fid_list[j];
	if ( f2_id == f1_id ) {
	  continue;
	}
	if ( (det_flag[f2_id] & bv1) != 0UL ) {
	  // 同時に検出された故障は除外
	  continue;
	}

	const vector<ymuint>& input_list2 = mAnalyzer.input_list(f2_id);
	bool intersect = check_intersect(input_list1, input_list2);
	if ( !intersect ) {
	  // 共通部分を持たない故障は独立
	  continue;
	}

	const FaultInfo& fi2 = mAnalyzer.fault_info(f2_id);
	const NodeValList& pi_suf_list2 = fi2.pi_sufficient_assignment();
	const NodeValList& ma_list2 = fi2.mandatory_assignment();

	if ( check_conflict(ma_list1, ma_list2) ) {
	  // 必要割当が衝突している．
	  fd1.mMaConflictList.push_back(f2_id);
	  continue;
	}

	if ( !check_conflict(pi_suf_list1, pi_suf_list2) ) {
	  // 外部入力における十分割当が両立している．
	  continue;
	}

	fd1.mCandList.push_back(f2_id);
      }
      fd1.mCandListSize = fd1.mCandList.size();
      nchg = fd1.mCandListSize;
    }
    else {
      // 二回目以降
      // 候補のうち，今回検出された故障を外す．
      ymuint wpos = 0;
      for (ymuint j = 0; j < fd1.mCandListSize; ++ j) {
	ymuint f2_id = fd1.mCandList[j];
	if ( (det_flag[f2_id] & bv1) != 0 ) {
	  continue;
	}
	if ( wpos != j ) {
	  fd1.mCandList[wpos] = f2_id;
	}
	++ wpos;
      }
      if ( wpos < fd1.mCandListSize ) {
	nchg += fd1.mCandListSize - wpos;
	fd1.mCandListSize = wpos;
      }
    }
    fd1.mDetCount += count_ones(bv1);
  }

  return nchg;
}

END_NAMESPACE_YM_SATPG_SA
