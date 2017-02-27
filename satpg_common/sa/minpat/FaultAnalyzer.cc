﻿
/// @file FaultAnalyzer.cc
/// @brief FaultAnalyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "FaultAnalyzer.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "TvMgr.h"
#include "TestVector.h"
#include "NodeValList.h"

#include "sa/StructSat.h"
#include "sa/FoCone.h"
#include "ValMap.h"

#include "sa/BackTracer.h"

#include "ym/RandGen.h"
#include "ym/StopWatch.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG_SA

BEGIN_NONAMESPACE

void
mark_tfo(const TpgNode* node,
	 HashSet<ymuint>& tfo_mark,
	 vector<const TpgNode*>& node_list)
{
  if ( tfo_mark.check(node->id()) ) {
    return;
  }
  tfo_mark.add(node->id());

  if ( node->is_ppo() ) {
    node_list.push_back(node);
  }

  ymuint no = node->fanout_num();
  for (ymuint i = 0; i < no; ++ i) {
    const TpgNode* onode = node->fanout(i);
    mark_tfo(onode, tfo_mark, node_list);
  }
}

void
mark_tfi(const TpgNode* node,
	 HashSet<ymuint>& tfi_mark,
	 vector<ymuint>& input_list)
{
  if ( tfi_mark.check(node->id()) ) {
    return;
  }
  tfi_mark.add(node->id());

  if ( node->is_ppi() ) {
    input_list.push_back(node->input_id());
  }
  else {
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      mark_tfi(inode, tfi_mark, input_list);
    }
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス FaultAnalyzer
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FaultAnalyzer::FaultAnalyzer()
{
  mVerbose = 0;
}

// @brief デストラクタ
FaultAnalyzer::~FaultAnalyzer()
{
}

// @brief verbose フラグを設定する．
// @param[in] verbose 表示を制御するフラグ
void
FaultAnalyzer::set_verbose(int verbose)
{
  mVerbose = verbose;
}

// @brief verbose フラグを得る．
int
FaultAnalyzer::verbose() const
{
  return mVerbose;
}

// @brief 初期化する．
// @param[in] network ネットワーク
// @param[in] tvmgr テストベクタのマネージャ
// @param[out] fault_list 検出された故障のリスト
void
FaultAnalyzer::init(const TpgNetwork& network,
		    TvMgr& tvmgr,
		    vector<const TpgFault*>& fault_list)
{
  StopWatch local_timer;
  local_timer.start();

  ymuint nn = network.node_num();
  mMaxNodeId = network.node_num();
  mMaxFaultId = network.max_fault_id();

  mInputListArray.clear();
  mInputListArray.resize(mMaxNodeId);

  mInputList2Array.clear();
  mInputList2Array.resize(mMaxNodeId);

  mFaultInfoArray.clear();
  mFaultInfoArray.resize(mMaxFaultId);

  mDomCheckCount = 0;

  ymuint f_all = 0;
  ymuint f_det = 0;
  ymuint f_red = 0;
  ymuint f_abt = 0;

  vector<bool> det_flag(mMaxFaultId, false);
  for (ymuint i = 0; i < nn; ++ i) {
    if ( verbose() > 1 ) {
      cout << "\r" << setw(6) << i << " / " << setw(6) << nn;
      cout.flush();
    }

    const TpgNode* node = network.node(i);
    if ( node->ffr_root() == node ) {
      // 故障箇所の TFI of TFI を node_set に記録する．
      vector<const TpgNode*> tfo_list;
      HashSet<ymuint> tfo_mark;
      mark_tfo(node, tfo_mark, tfo_list);

      // tfo_list の TFI に含まれる外部入力を mInputListArray に入れる．
      vector<ymuint>& input_list = mInputListArray[node->id()];
      HashSet<ymuint> tfi_mark;
      for (ymuint i = 0; i < tfo_list.size(); ++ i) {
	const TpgNode* node = tfo_list[i];
	mark_tfi(node, tfi_mark, input_list);
      }
      // ソートしておく．
      sort(input_list.begin(), input_list.end());

      // 故障箇所の TFI に含まれる入力番号を mInputList2Array に入れる．
      HashSet<ymuint> tfi_mark2;
      vector<ymuint>& input_list2 = mInputList2Array[node->id()];
      mark_tfi(node, tfi_mark2, input_list2);
      // ソートしておく．
      sort(input_list2.begin(), input_list2.end());
    }

    ymuint nf = node->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = node->fault(j);
      SatBool3 stat = analyze_fault(fault, tvmgr);
      ++ f_all;
      switch ( stat ) {
      case kB3True:
	fault_list.push_back(fault);
	++ f_det;
	det_flag[fault->id()] = true;
	break;

      case kB3False:
	++ f_red;
	break;

      case kB3X:
	++ f_abt;
      }
    }
  }

#if 0
  mOrigFidList.clear();
  mOrigFidList.reserve(f_det);
#endif
  for (ymuint i = 0; i < network.node_num(); ++ i) {
    const TpgNode* node = network.node(i);
#if 0
    ymuint ni = node->fanin_num();
    bool has_ncfault = false;
    for (ymuint j = 0; j < ni; ++ j) {
      const TpgFault* f0 = node->input_fault(0, j);
      if ( f0 != nullptr ) {
	if ( f0->is_rep() && det_flag[f0->id()] ) {
#if 0
	  // 代表故障で検出可能なら記録する．
	  mOrigFidList.push_back(f0->id());
#endif
	}
	if ( node->nval() == kVal0 && det_flag[f0->rep_fault()->id()] ) {
	  // 非制御値の故障で検出可能なものがあることを記録する．
	  has_ncfault = true;
	}
      }
      const TpgFault* f1 = node->input_fault(1, j);
      if ( f1 != nullptr ) {
	if ( f1->is_rep() && det_flag[f1->id()] ) {
#if 0
	  // 代表故障で検出可能なら記録する．
	  mOrigFidList.push_back(f1->id());
#endif
	}
	if ( node->nval() == kVal1 && det_flag[f1->rep_fault()->id()] ) {
	  // 非制御値の故障で検出可能なものがあることを記録する．
	  has_ncfault = true;
	}
      }
    }
    const TpgFault* f0 = node->output_fault(0);
    if ( f0 != nullptr && f0->is_rep() && det_flag[f0->id()] ) {
      if ( node->noval() != kVal0 || !has_ncfault ) {
#if 0
	// 非制御値でないか，入力側の非制御値の故障が検出可能でない時記録する．
	mOrigFidList.push_back(f0->id());
#endif
      }
    }
    const TpgFault* f1 = node->output_fault(1);
    if ( f1 != nullptr && f1->is_rep() && det_flag[f1->id()] ) {
      if ( node->noval() != kVal1 || !has_ncfault ) {
#if 0
	// 非制御値でないか，入力側の非制御値の故障が検出可能でない時記録する．
	mOrigFidList.push_back(f1->id());
#endif
      }
    }
#else
#if 0
    ymuint nf = network.node_fault_num(node->id());
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = network.node_fault(node->id(), j);
      if ( det_flag[fault->id()] ) {
	mOrigFidList.push_back(fault->id());
      }
    }
#endif
#endif
  }

  local_timer.stop();

  if ( verbose() > 0 ) {
    if ( verbose() > 1 ) {
      cout << endl;
    }
    cout << "Total " << setw(6) << f_all << " faults" << endl
	 << "Total " << setw(6) << f_det << " detected faults" << endl
	 << "Total " << setw(6) << f_red << " redundant faults" << endl
	 << "Total " << setw(6) << f_abt << " aborted faults" << endl
	 << "CPU time " << local_timer.time() << endl;
  }
}

// @brief 故障の解析を行う．
// @param[in] fault 故障
// @param[in] tvmgr テストベクタのマネージャ
SatBool3
FaultAnalyzer::analyze_fault(const TpgFault* fault,
			     TvMgr& tvmgr)
{
  ymuint f_id = fault->id();
  FaultInfo& fi = mFaultInfoArray[f_id];

  fi.mFault = fault;

  StructSat struct_sat(mMaxNodeId);
  const FoCone* focone = struct_sat.add_focone(fault, kVal1);

  vector<SatBool3> sat_model;
  SatBool3 sat_stat = struct_sat.check_sat(sat_model);
  if ( sat_stat == kB3True ) {
    // 割当結果から十分割当を求める．
    NodeValList& suf_list = fi.mSufficientAssignment;
    focone->get_suf_list(sat_model, fault, suf_list);
    NodeValList& pi_suf_list = fi.mPiSufficientAssignment;
    {
      ValMap val_map(focone->gvar_map(), focone->fvar_map(), sat_model);

      BackTracer backtracer(2, mMaxNodeId);
      backtracer(fault->tpg_onode(), NodeValList(), focone->output_list(), val_map, pi_suf_list);
      pi_suf_list.sort();
    }

    // テストベクタを作る．
    TestVector* tv = tvmgr.new_sa_vector();
    ymuint npi = pi_suf_list.size();
    for (ymuint i = 0; i < npi; ++ i) {
      NodeVal nv = pi_suf_list[i];
      const TpgNode* node = nv.node();
      ASSERT_COND ( node->is_ppi() );
      ymuint id = node->input_id();
      Val3 val = nv.val() ? kVal1 : kVal0;
      tv->set_input_val(id, val);
    }
    // X の部分をランダムに設定しておく
    tv->fix_x_from_random(mRandGen);

    fi.mTestVector = tv;

    // 必要割当を求める．
    NodeValList& ma_list = fi.mMandatoryAssignment;
    ymuint n = suf_list.size();
    for (ymuint i = 0; i < n; ++ i) {
      NodeVal nv = suf_list[i];

      NodeValList list1;
      const TpgNode* node = nv.node();
      bool bval = nv.val();
      list1.add(node, 0, !bval);
      if ( struct_sat.check_sat(list1) == kB3False ) {
	// node の値を反転したら検出できなかった．
	// -> この割当は必須割当
	ma_list.add(node, 0, bval);
      }
    }

    if ( suf_list.size() == ma_list.size() ) {
      fi.mSingleCube = true;
    }
  }

  return sat_stat;
}

// @brief 故障の情報をクリアする．
// @param[in] fid 故障番号
// @param[in] tv_mgr テストベクタを管理するクラス
//
// 非支配故障の情報をクリアすることでメモリを減らす．
void
FaultAnalyzer::clear_fault_info(ymuint fid,
				TvMgr& tv_mgr)
{
  ASSERT_COND( fid < mMaxFaultId );
  FaultInfo& fi = mFaultInfoArray[fid];
  fi.mMandatoryAssignment.clear();
  fi.mSufficientAssignment.clear();
  fi.mPiSufficientAssignment.clear();
  fi.mOtherSufListArray.clear();
  tv_mgr.delete_vector(fi.mTestVector);
}

// @brief ノード番号の最大値を得る．
ymuint
FaultAnalyzer::max_node_id() const
{
  return mMaxNodeId;
}

// @brief 故障番号の最大値を得る．
ymuint
FaultAnalyzer::max_fault_id() const
{
  return mMaxFaultId;
}

// @brief 故障を得る．
// @param[in] fid 故障番号
const TpgFault*
FaultAnalyzer::fault(ymuint fid) const
{
  ASSERT_COND( fid < mMaxFaultId );
  return mFaultInfoArray[fid].fault();
}

// @brief 個別の故障の情報を得る．
// @param[in] fid 故障番号
const FaultInfo&
FaultAnalyzer::fault_info(ymuint fid) const
{
  ASSERT_COND( fid < mMaxFaultId );
  return mFaultInfoArray[fid];
}

// @brief 故障のTFOのTFIに含まれる入力番号のリスト返す．
// @param[in] fid 故障番号
const vector<ymuint>&
FaultAnalyzer::input_list(ymuint fid) const
{
  ASSERT_COND( fid < mMaxFaultId );
  const TpgFault* fault = mFaultInfoArray[fid].fault();
  const TpgNode* node = fault->tpg_onode();
  const TpgNode* ffr_root = node->ffr_root();
  return mInputListArray[ffr_root->id()];
}

// @brief 故障のTFIに含まれる入力番号のリスト返す．
// @param[in] fid 故障番号
const vector<ymuint>&
FaultAnalyzer::input_list2(ymuint fid) const
{
  ASSERT_COND( fid < mMaxFaultId );
  const TpgFault* fault = mFaultInfoArray[fid].fault();
  const TpgNode* node = fault->tpg_onode();
  const TpgNode* ffr_root = node->ffr_root();
  return mInputList2Array[ffr_root->id()];
}

// @brief 等価故障を記録する．
void
FaultAnalyzer::add_eq_fault(ymuint fid1,
			    ymuint fid2)
{
  mFaultInfoArray[fid1].mEqList.push_back(fid2);
}

// @brief 支配故障を記録する．
void
FaultAnalyzer::add_dom_fault(ymuint fid1,
			     ymuint fid2)
{
  FaultInfo& fi1 = mFaultInfoArray[fid1];
  FaultInfo& fi2 = mFaultInfoArray[fid2];
  fi1.mDomList.push_back(fid2);
  fi1.mDomList.insert(fi1.mDomList.end(), fi2.mDomList.begin(), fi2.mDomList.end());
}

// @brief 故障の等価性をチェックする．
// @param[in] f1_id, f2_id 対象の故障
// @retval true f1_id と f2_id が等価だった．
// @retval false f1_id と f2_id は等価ではなかった．
//
// f1 を検出するパタン集合と f2 を検出するパタン集合
// が完全に一致するとき f1 と f2 が等価であると言う．
// f1 が f2 を支配し，f2 が f1 を支配することと同値
bool
FaultAnalyzer::check_equivalence(ymuint f1_id,
				 ymuint f2_id) const
{
  return check_dominance(f1_id, f2_id) && check_dominance(f2_id, f1_id);
}

BEGIN_NONAMESPACE

const TpgNode*
common_node(const TpgNode* node1,
	    const TpgNode* node2)
{
  ymuint id1 = node1->id();
  ymuint id2 = node2->id();
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
      if ( node1 == nullptr ) {
	return nullptr;
      }
      id1 = node1->id();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
      if ( node2 == nullptr ) {
	return nullptr;
      }
      id2 = node2->id();
    }
  }
}

END_NONAMESPACE

// @brief 故障の支配関係をチェックする．
// @param[in] f1_id, f2_id 対象の故障
// @retval true f1_id が f2_id を支配している．
// @retval false f1_id が f2_id を支配していない．
//
// f1 を検出するいかなるパタンも f2 を検出する時
// f1 が f2 を支配すると言う．
bool
FaultAnalyzer::check_dominance(ymuint f1_id,
			       ymuint f2_id) const
{
  StopWatch timer;
  timer.start();

  const FaultInfo& fi1 = fault_info(f1_id);
  const FaultInfo& fi2 = fault_info(f2_id);

  const TpgFault* f1 = fault(f1_id);
  const TpgFault* f2 = fault(f2_id);

  const TpgNode* fnode1 = f1->tpg_onode();
  const TpgNode* fnode2 = f2->tpg_onode();
  const TpgNode* dom_node = common_node(fnode1, fnode2);

  StructSat struct_sat(mMaxNodeId);

  // f1 の必要条件を追加する．
  const NodeValList& ma_list1 = fi1.mandatory_assignment();
  struct_sat.add_assignments(ma_list1);

  SatBool3 sat_stat = kB3X;

  if ( dom_node != nullptr ) {
    // 伝搬径路に共通な dominator がある時
    ++ mDomCheckCount;

    // f2 の故障を検出しない条件を追加
    struct_sat.add_focone(f2, dom_node, kVal0);

    sat_stat = struct_sat.check_sat();

    if ( sat_stat != kB3False ) {
      // f1 の故障を検出する条件を追加
      struct_sat.add_focone(f1, dom_node, kVal1);

      sat_stat = struct_sat.check_sat();

      if ( sat_stat != kB3False ) {
	// 共通部分の伝搬条件を追加する．
	struct_sat.add_focone(dom_node, kVal1);

	sat_stat = struct_sat.check_sat();
      }
    }
  }
  else {
    // f2 の故障を検出しない条件を追加
    struct_sat.add_focone(f2, kVal0);

    sat_stat = struct_sat.check_sat();
    if ( sat_stat != kB3False ) {
      // f1 の故障を検出する条件を追加
      struct_sat.add_focone(f1, kVal1);

      sat_stat = struct_sat.check_sat();
    }
  }

  timer.stop();
  USTime time = timer.time();

  if ( sat_stat == kB3False ) {
    mSuccessTime += time;
    if ( mSuccessMax.usr_time_usec() < time.usr_time_usec() ) {
      if ( time.usr_time() > 1.0 ) {
	cout << "UNSAT: " << f1 << ": " << f2 << "  " << time << endl;
      }
      mSuccessMax = time;
    }
    return true;
  }
  else if ( sat_stat == kB3True ) {
    mFailureTime += time;
    if ( mFailureMax.usr_time_usec() < time.usr_time_usec() ) {
      if ( time.usr_time() > 1.0 ) {
	cout << "SAT: " << f1 << ": " << f2 << "  " << time << endl;
      }
      mFailureMax = time;
    }
    return false;
  }
  else {
    mAbortTime += timer.time();
    if ( mAbortMax.usr_time_usec() < time.usr_time_usec() ) {
      if ( time.usr_time() > 1.0 ) {
	cout << "ABORT: " << f1 << ": " << f2 << "  " << time << endl;
      }
      mAbortMax = time;
    }
    return false;
  }
}

// @brief 故障の両立性をチェックする．
// @param[in] f1_id, f2_id 対象の故障
// @retval true f1 と f2 が両立する．
// @retval false f1 と f2 が衝突している．
//
// f1 を検出するパタン集合と f2 を検出するパタン集合
// の共通部分がからでない時 f1 と f2 は両立すると言う．
bool
FaultAnalyzer::check_compatibility(ymuint f1_id,
				   ymuint f2_id) const
{
  return false;
}

// @brief 処理時間の情報を出力する．
// @param[in] s 出力先のストリーム
void
FaultAnalyzer::print_stats(ostream& s) const
{
  s << "  CPU time (success)     " << mSuccessTime << "(MAX " << mSuccessMax << ")" << endl
    << "  CPU time (failure)     " << mFailureTime << "(MAX " << mFailureMax << ")" << endl
    << "  CPU time (abort)       " << mAbortTime   << "(MAX " << mAbortMax << ")" << endl
    << "  # of common dominator checkes " << mDomCheckCount << endl;
}

END_NAMESPACE_YM_SATPG_SA
