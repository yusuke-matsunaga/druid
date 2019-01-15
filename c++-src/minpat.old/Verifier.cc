
/// @file Verifier.cc
/// @brief Verifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Verifier.h"
#include "Fsim.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_DRUID_SA

//////////////////////////////////////////////////////////////////////
// クラス Verifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Verifier::Verifier()
{
}

// @brief デストラクタ
Verifier::~Verifier()
{
}

// @brief 与えられたパタンリストで全検出済み故障を検出できるか調べる．
// @param[in] fsim 故障シミュレータ
// @param[in] fault_list 故障のリスト
// @param[in] pat_list パタンのリスト
bool
Verifier::check(Fsim& fsim,
		const vector<const TpgFault*>& fault_list,
		const vector<const TestVector*>& pat_list)
{
  fsim.clear_skip(fault_list);

  // 検出された故障番号を入れるハッシュ表
  HashSet<ymuint> fhash;

  ymuint wpos = 0;
  fsim.clear_patterns();
  ymuint num = pat_list.size();
  for (ymuint rpos = 0; ; ++ rpos) {
    if ( rpos < num ) {
      const TestVector* tv = pat_list[rpos];
      fsim.set_pattern(wpos, tv);
      ++ wpos;
      if ( wpos < kPvBitLen ) {
	continue;
      }
    }
    else if ( wpos == 0 ) {
      break;
    }

    ymuint n = fsim.ppsfp();
    fsim.clear_patterns();
    wpos = 0;

    for (ymuint i = 0; i < n; ++ i) {
      const TpgFault* f = fsim.det_fault(i);
      // どのパタンで検出できたかは調べる必要はない．
      fhash.add(f->id());
    }
  }

  bool no_error = true;
  for (vector<const TpgFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    const TpgFault* fault = *p;
    if ( !fhash.find(fault->id()) ) {
      cout << "Error: " << fault << " has no patterns" << endl;
      no_error = false;
    }
  }

  return no_error;
}

END_NAMESPACE_DRUID_SA
