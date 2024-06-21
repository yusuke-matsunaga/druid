
/// @file DomCandGen.cc
/// @brief DomCandGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomCandGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

SizeType
get_max_fault_id(
  const vector<const TpgFault*>& fault_list
)
{
  SizeType max_id = 0;
  for ( auto f: fault_list ) {
    max_id = std::max(max_id, f->id());
  }
  return max_id;
}

END_NONAMESPACE


// @brief コンストラクタ
DomCandGen::DomCandGen(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list
) : mFaultList{fault_list},
    mMaxFaultId{get_max_fault_id(fault_list)},
    mTvList{tv_list},
    mFsim{network, fault_list, false, false},
    mHasPrevState{network.fault_type() == FaultType::TransitionDelay},
    mInputNum{network.input_num()},
    mDffNum{network.dff_num()},
    mWorkArray(mMaxFaultId)
{
  mFsim.set_skip_all();
  for ( auto f: fault_list ) {
    mFsim.clear_skip(f);
    auto& work = mWorkArray[f->id()];
    work.mPat = PV_ALL0;
    work.mHasDomCandList = false;
  }
}

// @brief デストラクタ
DomCandGen::~DomCandGen()
{
}

// @brief 支配故障の候補リストを作る．
void
DomCandGen::run(
  SizeType loop_limit,
  vector<vector<const TpgFault*>>& dom_cand_list
)
{
  std::mt19937 rg;
  vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  // 与えられたテストベクタを用いる．
  // これで各故障が最低1回は検出されるはず．
  for ( auto tv: mTvList ) {
    tv.fix_x_from_random(rg);
    tv_buff.push_back(tv);
    if ( tv_buff.size() == PV_BITLEN ) {
      do_fsim(tv_buff, dom_cand_list);
      tv_buff.clear();
    }
  }
  if ( tv_buff.size() > 0 ) {
    do_fsim(tv_buff, dom_cand_list);
  }

  // 乱数を用いて故障シミュレーションを行う．
  TestVector tv{mInputNum, mDffNum, mHasPrevState};
  for ( SizeType nc_count = 0; nc_count < loop_limit; ) {
    // 変化がなくなってから loop_limit 回過ぎたら終わる．
    tv_buff.clear();
    for ( auto _: Range(PV_BITLEN) ) {
      tv.set_from_random(rg);
      tv_buff.push_back(tv);
    }
    if ( do_fsim(tv_buff, dom_cand_list) ) {
      nc_count = 0;
    }
    else {
      ++ nc_count;
    }
  }
}

// @brief 一回の故障シミュレーションを行う．
bool
DomCandGen::do_fsim(
  const vector<TestVector>& tv_list,
  vector<vector<const TpgFault*>>& dom_cand_list
)
{
  bool changed = false;

  // 故障シミュレーションを行い，検出された故障を det_fault_list に入れる．
  // 結果の検出パタンを FaultInfo.mPat に入れる．
  // FaultInfo.mPat はクリアされていると仮定するので，
  // ここで代入を行わない故障に関しては 0 (PV_ALL0) になっているはず．
  vector<const TpgFault*> det_fault_list;
  mFsim.ppsfp(tv_list,
	      [&](const TpgFault* f, const DiffBitsArray& dba) {
		auto& fi = mWorkArray[f->id()];
		fi.mPat = dba.dbits_union();
		det_fault_list.push_back(f);
	      });

  // パタンに基づいて dom_cand_list を更新する．
  for ( auto f1: det_fault_list ) {
    auto& w1 = mWorkArray[f1->id()];
    auto pat1 = w1.mPat;
    auto& dst_list = dom_cand_list[f1->id()];
    SizeType n0 = dst_list.size();
    if ( !w1.mHasDomCandList ) {
      // まだ dom_cand_list がない．
      // det_fault_list に含まれる他の故障を対象に調べる．
      // それ以外の故障については考える必要がない．
      for ( auto f2: det_fault_list ) {
	if ( f1 == f2 ) {
	  continue;
	}
	auto& w2 = mWorkArray[f2->id()];
	auto pat2 = w2.mPat;
	if ( (pat1 & pat2) == pat1 ) {
	  // f1 が検出されて f2 が検出されるパタンがない．
	  dst_list.push_back(f2);
	}
      }
      w1.mHasDomCandList = true;
      changed = true;
    }
    else {
      // dom_cand_list のなかで (pat1 & pat2) != pat1 の故障を取り除く
      // 今回，検出されなかった故障のパタンは 0 なので取り除かれる．
      vector<const TpgFault*>::iterator rpos = dst_list.begin();
      vector<const TpgFault*>::iterator end = dst_list.end();
      vector<const TpgFault*>::iterator wpos = rpos;
      for ( ; rpos != end; ++ rpos ) {
	auto f2 = *rpos;
	auto& w2 = mWorkArray[f2->id()];
	auto pat2 = w2.mPat;
	if ( (pat1 & pat2) == pat1 ) {
	  if ( wpos != rpos ) {
	    *wpos = f2;
	  }
	  ++ wpos;
	}
      }
      if ( wpos != end ) {
	dst_list.erase(wpos, end);
	changed = true;
      }
    }
  }

  // 次回のシミュレーションに備えて mPat をクリアしておく．
  for ( auto f: det_fault_list ) {
    auto& w = mWorkArray[f->id()];
    w.mPat = PV_ALL0;
  }

  return changed;
}

END_NAMESPACE_DRUID
