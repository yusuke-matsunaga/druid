
/// @file MinPatDsatur2.cc
/// @brief MinPatDsatur2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatDsatur2.h"
#include "TpgNetwork.h"
#include "FaultAnalyzer.h"
#include "EqChecker.h"
#include "DomChecker.h"
#include "ConflictChecker.h"
#include "FgMgr.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG_SA

// @brief インスタンスを生成する関数
MinPat*
new_MinPatDsatur2()
{
  return new MinPatDsatur2();
}

//////////////////////////////////////////////////////////////////////
// クラス MinPatDsatur2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatDsatur2::MinPatDsatur2()
{
}

// @brief デストラクタ
MinPatDsatur2::~MinPatDsatur2()
{
}

// @brief 初期化を行う．
// @param[in] fid_list 検出された故障のリスト
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fsim2 2値の故障シミュレータ(検証用)
void
MinPatDsatur2::init(const vector<ymuint>& fid_list,
		    TvMgr& tvmgr,
		    Fsim& fsim2)
{
  // 代表故障のリスト
  vector<ymuint> rep_fid_list;
  {
    EqChecker checker(analyzer(), tvmgr, fsim2);
    checker.get_rep_faults(fid_list, rep_fid_list);
  }

  // 支配故障のリスト
  {
    mDomFidList.clear();
    DomChecker checker(analyzer(), tvmgr, fsim2);
    checker.get_dom_faults(rep_fid_list, mDomFidList);
  }

  ConflictChecker checker2(analyzer(), tvmgr, fsim2);
  checker2.analyze_conflict(mDomFidList);

  ymuint nf = mDomFidList.size();

  ymuint max_fault_id = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    ymuint fid = mDomFidList[i];
    if ( max_fault_id < fid ) {
      max_fault_id = fid;
    }
  }
  ++ max_fault_id;

  mFaultStructList.clear();
  mFaultStructList.resize(nf);
  mFaultMap.clear();
  mFaultMap.resize(max_fault_id);

  for (ymuint i = 0; i < nf; ++ i) {
    ymuint fid = mDomFidList[i];
    FaultStruct& fs = mFaultStructList[i];
    fs.mFaultId = fid;
    fs.mSelected = false;
    fs.mConflictList = checker2.conflict_list(fid);
    fs.mConflictNum = 0;
    fs.mConflictMap.resize(1, false);
    mFaultMap[fid] = i;
  }

  mFaultNum = nf;
  mRemainNum = nf;
}

// @brief 対象の全故障数を返す．
ymuint
MinPatDsatur2::fault_num()
{
  return mFaultNum;
}

// @brief 故障番号のリストを返す．
const vector<ymuint>&
MinPatDsatur2::fid_list()
{
  return mDomFidList;
}

// @brief 最初の故障を選ぶ．
ymuint
MinPatDsatur2::get_first_fault()
{
  ASSERT_COND( mRemainNum > 0 );

  // 最初は衝突数の多い故障を選ぶ．
  ymuint max_count = 0;
  ymuint max_pos = 0;
  ymuint fault_num = mFaultStructList.size();
  for (ymuint i = 0; i < fault_num; ++ i) {
    FaultStruct& fs = mFaultStructList[i];
    if ( max_count < fs.mConflictList.size() ) {
      max_count = fs.mConflictList.size();
      max_pos = i;
    }
  }
  mFaultStructList[max_pos].mSelected = true;
  -- mRemainNum;
  mPrevFpos = max_pos;
  mPrevGid = 0;
  return mFaultStructList[max_pos].mFaultId;
}

// @brief 次に処理すべき故障を選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] group_list 現在のグループリスト
//
// 故障が残っていなければ nullptr を返す．
ymuint
MinPatDsatur2::get_next_fault(FgMgr& fgmgr,
			      const vector<ymuint>& group_list)
{
  ASSERT_COND( mRemainNum > 0 );

  // 飽和度最大の故障を選ぶ．
  ymuint ng = fgmgr.group_num();
  ymuint fault_num = mFaultStructList.size();

  ymuint max_pos = 0;

  bool first = true;
  ymuint max_satur = 0;
  ymuint max_conf = 0;
  for (ymuint i = 0; i < fault_num; ++ i) {
    FaultStruct& fs = mFaultStructList[i];
    if ( fs.mSelected ) {
      continue;
    }
    if ( !fs.mConflictMap[mPrevGid] ) {
      FaultStruct& fs2 = mFaultStructList[mPrevFpos];
      for (ymuint j = 0; j < fs2.mConflictList.size(); ++ j) {
	ymuint fid = fs2.mConflictList[j];
	if ( fid == fs.mFaultId ) {
	  fs.mConflictMap[mPrevGid] = true;
	  ++ fs.mConflictNum;
	  break;
	}
      }
    }
    if ( first || max_satur < fs.mConflictNum ) {
      first = false;
      max_satur = fs.mConflictNum;
      max_pos = i;
      ymuint conf_num = 0;
      for (ymuint j = 0; j < fs.mConflictList.size(); ++ j) {
	ymuint fid = fs.mConflictList[j];
	if ( !mFaultStructList[mFaultMap[fid]].mSelected ) {
	  ++ conf_num;
	}
      }
      max_conf = conf_num;
    }
    else if ( max_satur == fs.mConflictNum ) {
      // 同点の場合には未選択の故障と多く衝突するものを選ぶ．
      ymuint conf_num = 0;
      for (ymuint j = 0; j < fs.mConflictList.size(); ++ j) {
	ymuint fid = fs.mConflictList[j];
	if ( !mFaultStructList[mFaultMap[fid]].mSelected ) {
	  ++ conf_num;
	}
      }
      if ( max_conf < conf_num ) {
	max_conf = conf_num;
	max_pos = i;
      }
    }
  }
  mFaultStructList[max_pos].mSelected = true;
  -- mRemainNum;
  mPrevFpos = max_pos;
  return mFaultStructList[max_pos].mFaultId;
}

// @brief 故障を追加するグループを選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] fid 故障番号
// @param[in] group_list 現在のグループリスト
//
// グループが見つからなければ fgmgr.group_num() を返す．
ymuint
MinPatDsatur2::find_group(FgMgr& fgmgr,
			  ymuint fid,
			  const vector<ymuint>& group_list)
{
  ymuint gid = MinPatBase::find_group(fgmgr, fid, group_list);
  mPrevGid = gid;
  ymuint ng = fgmgr.group_num();
  if ( gid == ng ) {
    // 新しいグループを作ることになる．
    for (ymuint i = 0; i < mFaultStructList.size(); ++ i) {
      FaultStruct& fs = mFaultStructList[i];
      if ( fs.mSelected ) {
	continue;
      }
      fs.mConflictMap.resize(ng + 1, false);
    }
  }
  return gid;
}

END_NAMESPACE_YM_SATPG_SA
