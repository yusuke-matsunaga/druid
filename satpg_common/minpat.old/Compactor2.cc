
/// @file Compactor.cc
/// @brief Compactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "Compactor.h"
#include "FgMgr.h"
#include "ym/HashSet.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_SA

BEGIN_NONAMESPACE

// グループの要素数の昇順に並べるための比較関数
struct GroupLt
{
  GroupLt(const FgMgr& fgmgr) :
    mFgMgr(fgmgr)
  {
  }

  bool
  operator()(ymuint left,
	     ymuint right)
  {
    return mFgMgr.fault_num(left) < mFgMgr.fault_num(right);
  }

  const FgMgr& mFgMgr;

};

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Compactor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Compactor::Compactor()
{
  mVerbose = 0;
  mFast = false;
  mPrintDetail = false;
}

// @brief デストラクタ
Compactor::~Compactor()
{
}

// @brief verbose フラグを設定する．
// @param[in] verbose 指定する値
void
Compactor::set_verbose(ymuint verbose)
{
  mVerbose = verbose;
}

// @brief print_detail フラグを設定する．
void
Compactor::set_print_detail(bool flag)
{
  mPrintDetail = flag;
}

// @brief 故障グループを圧縮する．
// @param[in] fmgr 故障グループマネージャ
// @param[in] max_node_id ノード番号の最大値
// @param[in] group_list もとのグループ番号リスト
// @param[in] fast 高速ヒューリスティック
// @param[out] new_group_list 圧縮後のグループ番号リスト
void
Compactor::run(FgMgr& fgmgr,
	       ymuint max_node_id,
	       const vector<ymuint>& group_list,
	       bool fast,
	       vector<ymuint>& new_group_list)
{
  mMaxNodeId = max_node_id;

  mFast = fast;

  new_group_list = group_list;

  if ( mPrintDetail ) {
    fgmgr.print_group_list(cout, new_group_list);
  }

#if 0
  phase0(fgmgr, new_group_list);
#endif

  phase1(fgmgr, new_group_list);

  for ( ; ; ) {

    if ( mPrintDetail ) {
      fgmgr.print_group_list(cout, new_group_list);
    }

    ymuint ng0 = new_group_list.size();
    phase2(fgmgr, new_group_list);

    if ( mPrintDetail ) {
      fgmgr.print_group_list(cout, new_group_list);
    }

    phase1(fgmgr, new_group_list);
    if ( new_group_list.size() == ng0 ) {
      break;
    }
  }
}

// @brief phase-0
// @param[in] fmgr 故障グループマネージャ
// @param[inout] group_list 選択されたグループ番号のリスト
//
// 他のグループに追加条件なしで移動できる故障を見つけ，移動させる．
void
Compactor::phase0(FgMgr& fgmgr,
		  vector<ymuint>& group_list)
{
  ymuint max_group_id = fgmgr.group_num();
#if 0
#if 0
  vector<bool> locked(max_group_id, false);
  for ( ; ; ) {
    ymuint min_gid = max_group_id;
    ymuint min_size = 0;
    ymuint ng = group_list.size();

    // 要素数が最小のグループを求める．
    for (ymuint i = 0; i < ng; ++ i) {
      ymuint gid = group_list[i];
      if ( locked[gid] ) {
	continue;
      }

      ymuint size = fgmgr.fault_list(gid).size();
      if ( min_size == 0 || min_size > size ) {
	min_size = size;
	min_gid = gid;
      }
    }
    if ( min_gid == max_group_id ) {
      // すべてのグループが調査済みだった．
      break;
    }

    // min_gid のグループの故障を他のグループへ移動できるか調べる．
    bool red = true;
    const vector<const TpgFault*>& fault_list = fgmgr.fault_list(min_gid);
    vector<const TpgFault*> del_list;
    for (ymuint i = 0; i < fault_list.size(); ++ i) {
      const TpgFault* fault = fault_list[i];

      SatEngine engine(string(), string(), nullptr);
      GvalCnf gval_cnf(mMaxNodeId);
      FvalCnf fval_cnf(mMaxNodeId, gval_cnf);

      // fault を検出できない条件 CNF を作る．
      fval_cnf.make_cnf(engine, fault, kVal0);

      // fault がマージできる他のグループを探す．
      bool found = false;
      for (ymuint j = 0; j < ng; ++ j) {
	ymuint gid = group_list[j];
	if ( gid == min_gid ) {
	  continue;
	}

	const NodeValList& suf_list0 = fgmgr.sufficient_assignment(gid);
	if ( engine.check_sat(gval_cnf, suf_list0) == kB3False ) {
	  fgmgr.add_fault(gid, fault);
	  del_list.push_back(fault);
	  break;
	}
      }
    }
    if ( del_list.size() == fault_list.size() ) {
      fgmgr.delete_group(min_gid);
      // group_list から min_gid を除く．
      ymuint wpos = 0;
      for (ymuint rpos = 0; rpos < ng; ++ rpos) {
	ymuint gid = group_list[rpos];
	if ( gid != min_gid ) {
	  if ( wpos != rpos ) {
	    group_list[wpos] = gid;
	  }
	  ++ wpos;
	}
      }
      group_list.erase(group_list.begin() + wpos, group_list.end());
    }
    else if ( !del_list.empty() ) {
      fgmgr.delete_fault(min_gid, del_list);
    }
    locked[min_gid] = true;
  }
#else
  vector<const TpgFault*> all_list;
  ymuint ng = group_list.size();
  for (ymuint i = 0; i < ng; ++ i) {
    ymuint gid = group_list[i];
    ymuint nf = fgmgr.fault_num(gid);
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = fgmgr.fault(gid, j);
      all_list.push_back(fault);
    }
  }

  MinCov mincov;

  mincov.set_size(all_list.size(), ng);

  for (ymuint i = 0; i < all_list.size(); ++ i) {
    const TpgFault* fault = all_list[i];

    GvalCnf gval_cnf(mMaxNodeId);
    FvalCnf fval_cnf(mMaxNodeId, gval_cnf);
    SatEngine engine(string(), string(), nullptr);

    engine.make_fval_cnf(fval_cnf, fault, kVal0);

    for (ymuint j = 0; j < ng; ++ j) {
      ymuint gid = group_list[j];
      const NodeValList& suf_list = fgmgr.sufficient_assignment(gid);
      if ( engine.check_sat(gval_cnf, suf_list) == kB3False ) {
	mincov.insert_elem(i, j);
      }
    }
  }

  vector<ymuint> col_set;
  ymuint new_ng = mincov.heuristic(col_set);
  cout << "new_ng = " << col_set.size() << endl;
#endif
#endif
}

// @brief phase-1
// @param[inout] group_list 選択されたグループ番号のリスト
//
// 他のグループに移動させることでグループを削除する．
void
Compactor::phase1(FgMgr& fgmgr,
		  vector<ymuint>& group_list)
{
  StopWatch local_timer;
  local_timer.start();

  if ( mVerbose > 0 ) {
    cout << "phase1:      initial # of groups = "
	 << setw(4) << group_list.size() << endl;
  }

  ymuint max_group_id = fgmgr.group_num();

  ymuint ng = group_list.size();
  vector<bool> deleted(max_group_id, false);
  for (ymuint gpos = 0; gpos < ng; ++ gpos) {

    // 現在の情報を tmp_group_list にコピーしておく
    vector<ymuint> tmp_group_list(ng);
    for (ymuint j = gpos + 1; j < ng; ++ j) {
      ymuint gid = group_list[j];
      ymuint gid2 = fgmgr.duplicate_group(gid);
      tmp_group_list[j] = gid2;
    }

    // fid の移動先を見つけるための作業用のグループ番号リスト
    // min_gid 以外のグループ番号をコピーしておく
    vector<ymuint> tmp_group_list1;
    tmp_group_list1.reserve(ng - gpos - 1);
    for (ymuint j = ng - 1; j > gpos; -- j) {
      ymuint gid = tmp_group_list[j];
      tmp_group_list1.push_back(gid);
    }

    // min_gid のグループの故障を他のグループへ移動できるか調べる．
    ymuint min_gid = group_list[gpos];
    bool red = true;
    ymuint nf = fgmgr.fault_num(min_gid);
    if ( nf > 100 ) {
      continue;
    }

    vector<ymuint> move_list(nf);
    for (ymuint fpos = 0; fpos < nf; ++ fpos) {
      ymuint fid = fgmgr.fault_id(min_gid, fpos);

      if ( mVerbose > 1 ) {
	cout << "\r"
	     << setw(4) << fpos << " / " << setw(4) << gpos << " / " << setw(4) << ng;
	cout.flush();
      }

      // fid を移動可能なグループを見つける．
      ymuint gid = fgmgr.find_group(fid, tmp_group_list1, mFast);
      if ( gid != fgmgr.group_num() ) {
	// 見つけた．
	fgmgr.add_fault(gid, fid);
	move_list[fpos] = gid;
      }
      else {
	// 見つからなかった．
	// このグループの処理は中止する．
	red = false;
	break;
      }
    }
    if ( red ) {
      // 変更を確定する．
      vector<ymuint> gmap(fgmgr.group_num());
      for (ymuint j = gpos + 1; j < ng; ++ j) {
	ymuint gid1 = group_list[j];
	ymuint gid2 = tmp_group_list[j];
	if ( fgmgr.fault_num(gid1) != fgmgr.fault_num(gid2) ) {
	  fgmgr.replace_group(gid1, gid2);
	  gmap[gid2] = gid1;
	}
	else {
	  fgmgr.delete_group(gid2);
	}
      }
      deleted[min_gid] = true;
      if ( mPrintDetail ) {
	for (ymuint i = 0; i < nf; ++ i) {
	  cout << "  MOVE " << fgmgr.fault_id(min_gid, i)
	       << " from #" << min_gid << " to #"
	       << gmap[move_list[i]] << endl;
	}
	cout << "  DELETE #" << min_gid << endl;
      }
    }
    else {
      // 変更を破棄する．
      for (ymuint j = gpos + 1; j < ng; ++ j) {
	ymuint gid = tmp_group_list[j];
	fgmgr.delete_group(gid);
      }
    }
  }

  // group_list から削除されたグループをを除く．
  ymuint wpos = 0;
  for (ymuint rpos = 0; rpos < ng; ++ rpos) {
    ymuint gid = group_list[rpos];
    if ( !deleted[gid] ) {
      if ( wpos != rpos ) {
	group_list[wpos] = gid;
      }
      ++ wpos;
    }
  }
  group_list.erase(group_list.begin() + wpos, group_list.end());

  local_timer.stop();
  if ( mVerbose > 0 ) {
    if ( mVerbose == 1 ) {
      cout << "           ";
    }
    cout << "  final # of groups   = " << setw(4) << group_list.size()
	 << ":  CPU time" << local_timer.time() << endl;
  }
}

// @brief phase-2
// @param[inout] group_list 選択されたグループ番号のリスト
//
// 他のグループに移動させることでグループを削除する．
void
Compactor::phase2(FgMgr& fgmgr,
		  vector<ymuint>& group_list)
{
  StopWatch local_timer;
  local_timer.start();

  if ( mVerbose > 0 ) {
    cout << "phase2:      initial # of groups = "
	 << setw(4) << group_list.size() << endl;
  }

  ymuint max_group_id = fgmgr.group_num();
  ymuint ng = group_list.size();

  // 要素数の昇順に並べる．
  vector<ymuint> tmp_group_list = group_list;
  sort(tmp_group_list.begin(), tmp_group_list.end(), GroupLt(fgmgr));

  HashSet<ymuint> fault_lock;
  for (ymuint gpos = 0; gpos < ng; ++ gpos) {
    ymuint min_gid = tmp_group_list[gpos];

    // 可能な限り故障を他のグループに移動する．
    ymuint nf = fgmgr.fault_num(min_gid);
    vector<ymuint> del_fid_list;
    del_fid_list.reserve(nf);
    for (ymuint fpos = 0; fpos < nf; ++ fpos) {
      ymuint fid = fgmgr.fault_id(min_gid, fpos);
      if ( fault_lock.check(fid) ) {
	continue;
      }
      fault_lock.add(fid);

      if ( mVerbose > 1 ) {
	cout << "\r"
	     << setw(4) << fpos << " / " << setw(4) << gpos << " / " << setw(4) << ng;
	cout.flush();
      }

      vector<ymuint> tmp_group_list1;
      tmp_group_list.reserve(ng - gpos - 1);
      for (ymuint j = gpos + 1; j < ng; ++ j) {
	ymuint gid = tmp_group_list[j];
	tmp_group_list1.push_back(gid);
      }

      ymuint gid = fgmgr.find_group(fid, tmp_group_list1, mFast);
      if ( gid != fgmgr.group_num() ) {
	fgmgr.add_fault(gid, fid);
	del_fid_list.push_back(fid);
      }
    }
    if ( !del_fid_list.empty() ) {
      fgmgr.delete_faults(min_gid, del_fid_list);
    }
  }

  local_timer.stop();
  if ( mVerbose > 0 ) {
    if ( mVerbose == 1 ) {
      cout << "           ";
    }
    cout << "  final # of groups   = " << setw(4) << group_list.size()
	 << ":  CPU time" << local_timer.time() << endl;
  }
}

END_NAMESPACE_YM_SATPG_SA
