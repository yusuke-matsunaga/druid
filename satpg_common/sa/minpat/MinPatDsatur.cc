
/// @file MinPatDsatur.cc
/// @brief MinPatDsatur の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatDsatur.h"
#include "TpgNetwork.h"
#include "FaultAnalyzer.h"
#include "EqChecker.h"
#include "DomChecker.h"
#include "FgMgr.h"
#include "sa/StructSat.h"


#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG_SA

// @brief インスタンスを生成する関数
MinPat*
new_MinPatDsatur()
{
  return new MinPatDsatur();
}

//////////////////////////////////////////////////////////////////////
// クラス MinPatDsatur
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatDsatur::MinPatDsatur()
{
}

// @brief デストラクタ
MinPatDsatur::~MinPatDsatur()
{
}

// @brief 初期化を行う．
// @param[in] fid_list 検出された故障のリスト
// @param[in] tvmgr テストベクタマネージャ
// @param[in] fsim2 2値の故障シミュレータ(検証用)
void
MinPatDsatur::init(const vector<ymuint>& fid_list,
		   TvMgr& tvmgr,
		   Fsim& fsim2)
{
  mMaxNodeId = analyzer().max_node_id();

  vector<ymuint> rep_fid_list;
  {
    EqChecker checker(analyzer(), tvmgr, fsim2);
    checker.get_rep_faults(fid_list, rep_fid_list);
  }

  mDomFidList.clear();
  DomChecker checker(analyzer(), tvmgr, fsim2);
  checker.get_dom_faults(rep_fid_list, mDomFidList);

  ymuint nf = mDomFidList.size();

  ymuint max_fault_id = 0;
  for (ymuint i = 0; i < nf; ++ i) {
    ymuint fid = mDomFidList[i];
    if ( max_fault_id < fid ) {
      max_fault_id = fid;;
    }
  }
  ++ max_fault_id;

  mFaultStructList.clear();
  mFaultStructList.resize(nf);
  mFaultMap.clear();
  mFaultMap.resize(max_fault_id);

  for (ymuint i = 0; i < nf; ++ i) {
    FaultStruct& fs = mFaultStructList[i];
    ymuint fid = mDomFidList[i];
    fs.mFaultId = fid;
    fs.mPatNum = checker.det_count(fid);
    fs.mSelected = false;
    fs.mConflictNum = 0;
    fs.mConflictMap.resize(1, false);
    fs.mPendingNum = 0;
    fs.mPendingMap.resize(1, false);
    mFaultMap[fid] = i;
  }
  mFaultNum = nf;
  mRemainNum = nf;

  mSimpleConfNum = 0;
  mSatConfNum = 0;
  mCompatNum = 0;
}

// @brief 対象の全故障数を返す．
ymuint
MinPatDsatur::fault_num()
{
  return mFaultNum;
}

// @brief 故障番号のリストを返す．
const vector<ymuint>&
MinPatDsatur::fid_list()
{
  return mDomFidList;
}

// @brief 最初の故障を選ぶ．
ymuint
MinPatDsatur::get_first_fault()
{
  ASSERT_COND( mRemainNum > 0 );

  // 最初は同時検出故障数の少ない故障を選ぶ．
  ymuint min_fid = 0;
  ymuint min_count = 0;
  ymuint min_pos = 0;
  ymuint fault_num = mFaultStructList.size();
  for (ymuint i = 0; i < fault_num; ++ i) {
    FaultStruct& fs = mFaultStructList[i];
    ymuint fnum = fs.mPatNum;
    if ( min_count == 0 || min_count > fnum ) {
      min_count = fnum;
      min_fid = fs.mFaultId;
      min_pos = i;
    }
  }
  mPrevGid = 0;
  mFaultStructList[min_pos].mSelected = true;
  -- mRemainNum;
  return min_fid;
}

// @brief 次に処理すべき故障を選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] group_list 現在のグループリスト
ymuint
MinPatDsatur::get_next_fault(FgMgr& fgmgr,
			     const vector<ymuint>& group_list)
{
  if ( mRemainNum == 0 ) {
    if ( verbose() ) {
      cout << endl;
      cout << "SimpleConfNum: " << mSimpleConfNum << endl
	   << "SatConfNum:    " << mSatConfNum << endl
	   << "CompatNum:     " << mCompatNum << endl;
    }
    return 0;
  }

  // 飽和度最大の故障を選ぶ．
  ymuint ng = fgmgr.group_num();
  ymuint fault_num = mFaultStructList.size();

  ymuint max_pos = 0;

  bool first = true;
  ymuint max_satur = 0;
  for (ymuint i = 0; i < fault_num; ++ i) {
    FaultStruct& fs = mFaultStructList[i];
    if ( fs.mSelected ) {
      continue;
    }
    if ( !fs.mConflictMap[mPrevGid] && !fs.mPendingMap[mPrevGid] ) {
      fs.mPendingMap[mPrevGid] = true;
      ++ fs.mPendingNum;
    }
    if ( first || max_satur < fs.mConflictNum ) {
      first = false;
      max_satur = fs.mConflictNum;
      max_pos = i;
    }
  }

  for ( ; ; ) {
    ymuint max2 = max_satur;
    ymuint max2_pos = 0;
    for (ymuint i = 0; i < fault_num; ++ i) {
      FaultStruct& fs = mFaultStructList[i];
      if ( fs.mSelected ) {
	continue;
      }
      if ( max2 < fs.mConflictNum + fs.mPendingNum ) {
	max2 = fs.mConflictNum + fs.mPendingNum;
	max2_pos = i;
      }
    }
    if ( max2 > max_satur ) {
      ymuint slack = max2 - max_satur;
      FaultStruct& fs = mFaultStructList[max2_pos];

      StructSat struct_sat(mMaxNodeId);

      ymuint fid = fs.mFaultId;
      const TpgFault* fault = analyzer().fault(fid);
      struct_sat.add_focone(fault, kVal1);

      const NodeValList& ma_list = analyzer().fault_info(fid).mandatory_assignment();
      for (ymuint gid = 0; gid < ng; ++ gid) {
	if ( fs.mPendingMap[gid] ) {
	  fs.mPendingMap[gid] = false;
	  -- fs.mPendingNum;
	  const NodeValList& suf_list0 = fgmgr.sufficient_assignment(gid);
	  if ( struct_sat.check_sat(ma_list) == kB3False ) {
	    ++ fs.mConflictNum;
	    fs.mConflictMap[gid] = true;
	    ++ mSimpleConfNum;
	  }
	  else if ( struct_sat.check_sat(suf_list0) == kB3False ) {
	    ++ fs.mConflictNum;
	    fs.mConflictMap[gid] = true;
	    ++ mSatConfNum;
	  }
	  else {
	    ++ mCompatNum;
	    -- slack;
	    if ( slack == 0 ) {
	      break;
	    }
	  }
	}
      }
      if ( slack > 0 ) {
	max_satur = fs.mConflictNum;
	max_pos = max2_pos;
      }
    }
    else {
      break;
    }
  }

  mFaultStructList[max_pos].mSelected = true;
  -- mRemainNum;
  return mFaultStructList[max_pos].mFaultId;
}

// @brief 故障を追加するグループを選ぶ．
// @param[in] fgmgr 故障グループを管理するオブジェクト
// @param[in] fid 故障番号
// @param[in] group_list 現在のグループリスト
//
// グループが見つからなければ fgmgr.group_num() を返す．
ymuint
MinPatDsatur::find_group(FgMgr& fgmgr,
			 ymuint fid,
			 const vector<ymuint>& group_list)
{
  ymuint gid = MinPatBase::find_group(fgmgr, fid, group_list);
  mPrevGid = gid;
  ymuint ng = fgmgr.group_num();
  if ( gid == ng ) {
    for (ymuint i = 0; i < mFaultStructList.size(); ++ i) {
      FaultStruct& fs = mFaultStructList[i];
      if ( fs.mSelected ) {
	continue;
      }
      fs.mConflictMap.resize(ng + 1, false);
      fs.mPendingMap.resize(ng + 1, false);
    }
  }
  return ng;
}

END_NAMESPACE_YM_SATPG_SA
