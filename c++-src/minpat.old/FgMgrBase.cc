
/// @file FgMgrBase.cc
/// @brief FgMgrBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "FgMgrBase.h"

#include "StructEnc.h"
#include "FoCone.h"

#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID_SA

bool verify_add_fault = false;

//////////////////////////////////////////////////////////////////////
// クラス FgMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障グループのリストを出力する．
// @param[in] s 出力先のストリーム
// @param[in] group_list グループ番号のリスト
void
FgMgr::print_group_list(ostream& s,
			const vector<ymuint>& group_list) const
{
  ymuint ng = group_list.size();
  for (ymuint i = 0; i < ng; ++ i) {
    ymuint gid = group_list[i];
    s << setw(4) << i << ": Group#" << gid;
    ymuint nf = fault_num(gid);
    for (ymuint j = 0; j < nf; ++ j) {
      ymuint fid = fault_id(gid, j);
      s << " " << fid;
    }
    s << endl;
  }
  s << endl;
}


//////////////////////////////////////////////////////////////////////
// クラス FgMgrBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_node_id ノード番号の最大値 + 1
// @param[in] analyzer 故障解析器
FgMgrBase::FgMgrBase(ymuint max_node_id,
		     const FaultAnalyzer& analyzer) :
  mMaxNodeId(max_node_id),
  mAnalyzer(analyzer)
{
  clear_count();
}

// @brief デストラクタ
FgMgrBase::~FgMgrBase()
{
  clear();
}

// @brief クリアする．
void
FgMgrBase::clear()
{
  for (ymuint gid = 0; gid < mGroupList.size(); ++ gid) {
    delete mGroupList[gid];
  }
  mGroupList.clear();
}

// @brief 現在のグループ数を返す．
ymuint
FgMgrBase::group_num() const
{
  return mGroupList.size();
}

// @brief 新しいグループを作る．
// @param[in] fid 故障番号
// @return グループ番号を返す．
//
// fid のみを要素に持つ．
ymuint
FgMgrBase::new_group(ymuint fid)
{
  FaultGroup* fg = _new_group();
  const FaultInfo& fi = _fault_info(fid);
  const NodeValList& suf_list = fi.sufficient_assignment();
  const NodeValList& ma_list = fi.mandatory_assignment();
  fg->add_fault(fid, suf_list, ma_list);
  return fg->id();
}

// @brief グループを複製する．
// @param[in] src_gid 複製元のグループ番号
// @return 新しいグループ番号を返す．
ymuint
FgMgrBase::duplicate_group(ymuint src_gid)
{
  FaultGroup* src_fg = _fault_group(src_gid);
  FaultGroup* dst_fg = _new_group();
  dst_fg->copy(*src_fg);
  return dst_fg->id();
}

// @brief グループを置き換える．
// @param[in] old_gid 置き換え対象のグループ番号
// @param[in] new_gid 置き換えるグループ番号
//
// new_gid は削除される．
void
FgMgrBase::replace_group(ymuint old_gid,
			 ymuint new_gid)
{
  delete_group(old_gid);

  FaultGroup* new_fg = _fault_group(new_gid);
  new_fg->set_id(old_gid);
  mGroupList[old_gid] = new_fg;
  mGroupList[new_gid] = nullptr;
}

// @brief グループを削除する．
// @param[in] gid グループ番号
void
FgMgrBase::delete_group(ymuint gid)
{
  FaultGroup* fg = _fault_group(gid);
  delete fg;
  mGroupList[gid] = nullptr;
}

// @brief 新たな条件なしで追加できる既存グループを見つける．
// @param[in] fid 対象の故障番号
// @param[in] group_list 探索対象のグループ番号のリスト
// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
// @param[out] gid_list 対象のグループ番号を収めるリスト
// @return 最初のグループ番号を返す．
//
// 見つからない場合は group_num() を返す．
// gid_list は first_hit == true の時，意味を持たない．
ymuint
FgMgrBase::find_dom_group(ymuint fid,
			  const vector<ymuint>& group_list,
			  bool first_hit,
			  vector<ymuint>& gid_list)
{
  StructEnc struct_sat(max_node_id());
  const TpgFault* fault = _fault(fid);
  // fault が検出できない条件を追加
  struct_sat.add_focone(fault, kVal0);

  ymuint first_gid = group_num();
  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];
    const NodeValList& suf_list0 = sufficient_assignment(gid);
    if ( struct_sat.check_sat(suf_list0) == kB3False ) {
      // suf_list0 の元では必ず検出できる
      if ( first_gid == group_num() ) {
	first_gid = gid;
	if ( first_hit ) {
	  break;
	}
      }
      gid_list.push_back(gid);
    }
  }

  return first_gid;
}

// @brief 追加できる既存グループを見つける．
// @param[in] fid0 対象の故障番号
// @param[in] group_list 探索対象のグループ番号のリスト
// @param[in] fast 高速ヒューリスティック
// @param[in] first_hit 最初のグループのみを求めるとき true にするフラグ
// @param[out] gid_list 対象のグループ番号を収めるリスト
// @return 最初のグループ番号を返す．
//
// 見つからない場合は group_num() を返す．
// gid_list は first_hit == true の時，意味を持たない．
ymuint
FgMgrBase::find_group(ymuint fid0,
		      const vector<ymuint>& group_list,
		      bool fast,
		      bool first_hit,
		      vector<ymuint>& gid_list)
{
  StopWatch local_timer;
  local_timer.start();

  ymuint first_gid = group_num();

  StructEnc struct_sat0(max_node_id());

  const FaultInfo& fi0 = _fault_info(fid0);

  // fi0 の必要割当を追加
  const NodeValList ma_list0 = fi0.mandatory_assignment();
  struct_sat0.add_assignments(ma_list0);

  if ( !fi0.single_cube() ) {
    // fault を検出する CNF を生成
    const TpgFault* f0 = _fault(fid0);
    struct_sat0.add_focone(f0, kVal1);
  }

  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];

    if ( check_conflict_cache(gid, fid0) ) {
      continue;
    }

    { // グループの十分割当が成り立っていたら両立している．
      const NodeValList& suf_list1 = sufficient_assignment(gid);
      if ( struct_sat0.check_sat(suf_list1) == kB3True ) {
	if ( first_gid == group_num() ) {
	  first_gid = gid;
	  if ( first_hit ) {
	    break;
	  }
	}
	gid_list.push_back(gid);
	continue;
      }
    }
    if ( fast ) {
      continue;
    }

    { // グループの必要割当が成り立たなかったら衝突している．
      const NodeValList& ma_list1 = mandatory_assignment(gid);
      if ( struct_sat0.check_sat(ma_list1) == kB3False ) {
	add_conflict_cache(gid, fid0);
	continue;
      }
    }

    ++ mCheckCount;

    // 簡易検査ではわからなかったので正式に調べる．
    StructEnc struct_sat(max_node_id());

    // fid0 の必要割当を追加
    struct_sat.add_assignments(ma_list0);
    // グループの必要割当を追加
    struct_sat.add_assignments(mandatory_assignment(gid));

    ymuint fnum = 0;

    if ( !fi0.single_cube() ) {
      // fid0 を検出する条件を追加
      const TpgFault* f0 = fi0.fault();
      struct_sat.add_focone(f0, kVal1);
      ++ fnum;
    }

    ymuint nf = fault_num(gid);
    for (ymuint i = 0; i < nf; ++ i) {
      ymuint fid1 = fault_id(gid, i);
      const FaultInfo& fi1 = _fault_info(fid1);
      if ( !fi1.single_cube() ) {
	// fid1 の検出条件を生成
	const TpgFault* f1 = fi1.fault();
	struct_sat.add_focone(f1, kVal1);
	++ fnum;
      }
    }

    mFsum += fnum;
    if ( mFmax < fnum ) {
      mFmax = fnum;
    }
    ++ mMnum;

    if ( struct_sat.check_sat() == kB3True ) {
      ++ mFoundCount;
      if ( first_gid == group_num() ) {
	first_gid = gid;
	if ( first_hit ) {
	  break;
	}
      }
      gid_list.push_back(gid);
    }
    else {
      add_conflict_cache(gid, fid0);
    }
  }

  local_timer.stop();
  mCheckTime += local_timer.time();

  return first_gid;
}

// @brief 追加できる既存グループを見つけて追加する．
// @param[in] fid 対象の故障番号
// @param[in] group_list 探索対象のグループ番号のリスト
// @param[in] fast 高速ヒューリスティック
// @return 見つかったグループ番号を返す．
//
// 見つからない場合は group_num() を返す．
ymuint
FgMgrBase::find_group2(ymuint fid0,
		       const vector<ymuint>& group_list,
		       bool fast)
{
  StopWatch local_timer;
  local_timer.start();

  StructEnc struct_sat0(max_node_id());

  const FaultInfo& fi0 = _fault_info(fid0);

  // fi0 の必要割当を追加
  const NodeValList ma_list0 = fi0.mandatory_assignment();
  struct_sat0.add_assignments(ma_list0);

  const FoCone* focone0 = nullptr;
  if ( !fi0.single_cube() ) {
    // f0 を検出する CNF を生成
    const TpgFault* f0 = fi0.fault();
    focone0 = struct_sat0.add_focone(f0, kVal1);
  }

  ymuint ans_gid = group_num();
  for (ymuint i = 0; i < group_list.size(); ++ i) {
    ymuint gid = group_list[i];

    if ( check_conflict_cache(gid, fid0) ) {
      continue;
    }

    { // グループの十分割当が成り立っていたら両立している．
      const NodeValList& suf_list1 = sufficient_assignment(gid);
      vector<SatBool3> sat_model;
      if ( struct_sat0.check_sat(suf_list1, sat_model) == kB3True ) {
	FaultGroup* fg = _fault_group(gid);
	if ( fi0.single_cube() ) {
	  fg->add_fault(fid0, ma_list0, ma_list0);
	}
	else {
	  NodeValList suf_list;
	  focone0->get_suf_list(sat_model, _fault(fid0), suf_list);
	  fg->add_fault(fid0, suf_list, ma_list0);
	}
	if ( verify_add_fault) {
	  StructEnc struct_sat(max_node_id());
	  // mSufList 単独で充足可能か調べておく．
	  if ( struct_sat.check_sat(fg->sufficient_assignment()) != kB3True ) {
	    cout << "Error in FaultGroup::add_fault(2)" << endl
		 << "  mSufList inconsistent" << endl;
	  }
	  if ( !check_sufficient_assignment(gid) ) {
	    cout << "Error in sufficient_assignment at add_fault(2)" << endl;
	  }
	}
	ans_gid = gid;
	break;
      }
    }
    if ( fast ) {
      continue;
    }

    { // グループの必要割当が成り立たなかったら衝突している．
      const NodeValList& ma_list1 = mandatory_assignment(gid);
      if ( struct_sat0.check_sat(ma_list1) == kB3False ) {
	add_conflict_cache(gid, fid0);
	continue;
      }
    }

    ++ mCheckCount;

    // 簡易検査ではわからなかったので正式に調べる．
    StructEnc struct_sat(max_node_id());

    // fid0 の必要割当を追加
    struct_sat.add_assignments(ma_list0);
    // グループの必要割当を追加
    struct_sat.add_assignments(mandatory_assignment(gid));

    ymuint fnum = 0;

    const FoCone* focone0 = nullptr;
    if ( !fi0.single_cube() ) {
      // fid0 を検出する条件を追加
      const TpgFault* f0 = fi0.fault();
      focone0 = struct_sat.add_focone(f0, kVal1);
      ++ fnum;
    }

    ymuint nf = fault_num(gid);
    vector<const FoCone*> focone_array(nf);
    for (ymuint i = 0; i < nf; ++ i) {
      ymuint fid1 = fault_id(gid, i);
      const FaultInfo& fi1 = _fault_info(fid1);
      if ( !fi1.single_cube() ) {
	// fid1 の検出条件を生成
	const TpgFault* f1 = fi1.fault();
	focone_array[i] = struct_sat.add_focone(f1, kVal1);
	++ fnum;
      }
    }

    mFsum += fnum;
    if ( mFmax < fnum ) {
      mFmax = fnum;
    }
    ++ mMnum;

    vector<SatBool3> sat_model;
    if ( struct_sat.check_sat(sat_model) == kB3True ) {
      ++ mFoundCount;

      FaultGroup* fg = _fault_group(gid);

      for (ymuint i = 0; i < nf; ++ i) {
	ymuint fid1 = fg->fault_id(i);
	const FaultInfo& fi1 = _fault_info(fid1);
	if ( !fi1.single_cube() ) {
	  NodeValList suf_list;
	  focone_array[i]->get_suf_list(sat_model, fi1.fault(), suf_list);
	  fg->set_suf_list(i, suf_list);
	}
      }

      fg->update();

      if ( verify_add_fault ) {
	StructEnc struct_sat(max_node_id());
	// mSufList 単独で充足可能か調べておく．
	if ( struct_sat.check_sat(fg->sufficient_assignment()) != kB3True ) {
	  cout << "Error in FaultGroup::update()" << endl
	       << "  mSufList inconsistent" << endl;
	}
	if ( !check_sufficient_assignment(gid) ) {
	  cout << "Error in sufficient_assignment at updatet()" << endl;
	}
      }

      if ( fi0.single_cube() ) {
	fg->add_fault(fid0, ma_list0, ma_list0);
      }
      else {
	NodeValList suf_list;
	focone0->get_suf_list(sat_model, _fault(fid0), suf_list);
	fg->add_fault(fid0, suf_list, ma_list0);
      }
      ans_gid = gid;

      if ( verify_add_fault ) {
	StructEnc struct_sat(max_node_id());
	// mSufList 単独で充足可能か調べておく．
	if ( struct_sat.check_sat(fg->sufficient_assignment()) != kB3True ) {
	  cout << "Error in FaultGroup::update()" << endl
	       << "  mSufList inconsistent" << endl;
	}
	if ( !check_sufficient_assignment(gid) ) {
	  cout << "Error in sufficient_assignment at updatet()" << endl;
	}
      }

      break;
    }
    else {
      add_conflict_cache(gid, fid0);
    }
  }

  local_timer.stop();
  mCheckTime += local_timer.time();

  return ans_gid;
}

// @brief 新しいグループを作る．
// @return グループを返す．
FgMgrBase::FaultGroup*
FgMgrBase::_new_group()
{
  FaultGroup* fg = nullptr;
  for (ymuint i = 0; i < mGroupList.size(); ++ i) {
    if ( mGroupList[i] == nullptr ) {
      fg = new FaultGroup(i);
      mGroupList[i] = fg;
      break;
    }
  }
  if ( fg == nullptr ) {
    ymuint id = mGroupList.size();
    fg = new FaultGroup(id);
    mGroupList.push_back(fg);
  }
  return fg;
}

// @brief 複数故障の検出検査回数
ymuint
FgMgrBase::mfault_num() const
{
  return mMnum;
}

// @brief 複数故障の平均多重度
double
FgMgrBase::mfault_avg() const
{
  if ( mMnum == 0 ) {
    return 0.0;
  }
  return static_cast<double>(mFsum) / static_cast<double>(mMnum);
}

// @brief 複数故障の最大値
ymuint
FgMgrBase::mfault_max() const
{
  return mFmax;
}

// @brief チェック回数
ymuint
FgMgrBase::check_count() const
{
  return mCheckCount;
}

// @brief チェック時間
USTime
FgMgrBase::check_time() const
{
  return mCheckTime;
}

// @brief 成功回数
ymuint
FgMgrBase::found_count() const
{
  return mFoundCount;
}

// @brief 統計データをクリアする．
void
FgMgrBase::clear_count()
{
  mMnum = 0;
  mFsum = 0;
  mFmax = 0;
  mCheckCount = 0;
  mFoundCount = 0;
  mCheckTime.set(0.0, 0.0, 0.0);
}

// @brief [デバッグ用] sufficient_assignment が正しいか調べる．
bool
FgMgrBase::check_sufficient_assignment(ymuint gid)
{
  // suf_list が正しいか検証する．
  bool error = false;
  const NodeValList& suf_list = sufficient_assignment(gid);
  for (ymuint i = 0; i < fault_num(gid); ++ i) {
    ymuint fid = fault_id(gid, i);
    StructEnc struct_sat(max_node_id());
    // gid の十分条件を追加
    struct_sat.add_assignments(suf_list);
    // fid を検出しない条件を追加
    const TpgFault* f = _fault(fid);
    struct_sat.add_focone(f, kVal0);
    // 十分条件が正しければこの SAT 問題は充足不能のはず．
    if ( struct_sat.check_sat() != kB3False ) {
      cout << "ERROR in fault group#" << gid << ": "
	   << f->str() << " is not detected with suf_list" << endl;
      error = true;
    }
    const FaultInfo& fi = _fault_info(fid);
    const vector<ymuint>& dom_list = fi.dom_list();
    for (ymuint j = 0; j < dom_list.size(); ++ j) {
      ymuint fid1 = dom_list[j];
      StructEnc struct_sat1(max_node_id());
      // gid の十分条件を追加
      struct_sat1.add_assignments(suf_list);
      // fid1 を検出しない条件を追加
      const TpgFault* f1 = _fault(fid1);
      struct_sat1.add_focone(f1, kVal0);
      // 十分条件が正しければこの SAT 問題は充足不能のはず．
      if ( struct_sat1.check_sat() != kB3False ) {
	cout << "ERROR in fault group#" << gid << ": "
	     << f1->str() << " is not detected with suf_list" << endl;
	error = true;
      }
    }
  }

  return !error;
}


//////////////////////////////////////////////////////////////////////
// クラス FgMgrBase::FaultGroup
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
FgMgrBase::FaultGroup::FaultGroup(ymuint id) :
  mId(id)
{
  mCplxNum = 0;
}

// @brief デストラクタ
FgMgrBase::FaultGroup::~FaultGroup()
{
}

// @brief ID番号を返す．
ymuint
FgMgrBase::FaultGroup::id() const
{
  return mId;
}

// 故障数を返す．
ymuint
FgMgrBase::FaultGroup::fault_num() const
{
  return mFaultDataList.size();
}

// single cube でない故障数を返す．
ymuint
FgMgrBase::FaultGroup::complex_fault_num() const
{
  return mCplxNum;
}

// 故障を返す．
ymuint
FgMgrBase::FaultGroup::fault_id(ymuint pos) const
{
  ASSERT_COND( pos < fault_num() );
  return mFaultDataList[pos].mFaultId;
}

// @brief 十分割当を返す．
const NodeValList&
FgMgrBase::FaultGroup::sufficient_assignment() const
{
  return mSufList;
}

// @brief 必要割当を返す．
const NodeValList&
FgMgrBase::FaultGroup::mandatory_assignment() const
{
  return mMaList;
}

// @brief 衝突キャッシュに登録する．
void
FgMgrBase::FaultGroup::add_conflict_cache(ymuint fid)
{
  mConflictCache.add(fid);
}

// @brief 衝突キャッシュを調べる．
bool
FgMgrBase::FaultGroup::check_conflict_cache(ymuint fid) const
{
  return mConflictCache.check(fid);
}

// @brief ID番号以外の内容をコピーする
void
FgMgrBase::FaultGroup::copy(const FaultGroup& dst)
{
  mFaultDataList = dst.mFaultDataList;
  mCplxNum = dst.mCplxNum;
  mSufList = dst.mSufList;
  mMaList = dst.mMaList;

  mConflictCache.clear();
  for (HashSetIterator<ymuint> p = dst.mConflictCache.begin();
       p != dst.mConflictCache.end(); ++ p) {
    ymuint fid = p.key();
    mConflictCache.add(fid);
  }
}

// @brief ID番号をセットする．
void
FgMgrBase::FaultGroup::set_id(ymuint id)
{
  mId = id;
}

// @brief 故障を追加する．
void
FgMgrBase::FaultGroup::add_fault(ymuint fid,
				 const NodeValList& suf_list,
				 const NodeValList& ma_list)
{
  mFaultDataList.push_back(FaultData(fid, suf_list, ma_list));
  mSufList.merge(suf_list);
  mMaList.merge(ma_list);
}

// @brief 故障を削除する．
void
FgMgrBase::FaultGroup::delete_faults(const vector<ymuint>& fid_list)
{
  // 削除対象の故障番号を持つハッシュ表
  HashSet<ymuint> fault_hash;
  for (ymuint i = 0; i < fid_list.size(); ++ i) {
    ymuint fid = fid_list[i];
    fault_hash.add(fid);
  }

  ymuint nf = fault_num();
  ymuint wpos = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    ymuint fid = fault_id(i);
    if ( fault_hash.check(fid) ) {
      continue;
    }
    if ( wpos != i ) {
      mFaultDataList[wpos] = mFaultDataList[i];
    }
    ++ wpos;
  }
  mFaultDataList.erase(mFaultDataList.begin() + wpos, mFaultDataList.end());

  mConflictCache.clear();

  update();
}

// @brief 故障の十分割当リストを設定する．
void
FgMgrBase::FaultGroup::set_suf_list(ymuint pos,
				    const NodeValList& suf_list)
{
  ASSERT_COND( pos < fault_num() );
  mFaultDataList[pos].mSufList = suf_list;
}

// @brief 故障リストが変更された時の更新処理を行う．
void
FgMgrBase::FaultGroup::update()
{
  mSufList.clear();
  mMaList.clear();
  ymuint nf = fault_num();
  ymuint wpos = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    mSufList.merge(mFaultDataList[i].mSufList);
    mMaList.merge(mFaultDataList[i].mMaList);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス FgMgrBase::FaultData
//////////////////////////////////////////////////////////////////////

// コンストラクタ
FgMgrBase::FaultGroup::FaultData::FaultData(ymuint fid,
					    const NodeValList& suf_list,
					    const NodeValList& ma_list) :
  mFaultId(fid),
  mSufList(suf_list),
  mMaList(ma_list)
{
}

END_NAMESPACE_DRUID_SA
