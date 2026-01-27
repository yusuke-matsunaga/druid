
/// @file Verifier.cc
/// @brief Verifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Verifier.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Verifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Verifier::Verifier(
  const TpgNetwork& network,
  bool has_previous_state,
  bool has_x
)
{
  mFsim.initialize(network, has_previous_state, has_x);
}

// @brief テストパタンが正しいかチェックする．
bool
Verifier::check(
  const vector<TpgFault>& fault_list,
  const vector<TestVector>& tv_list
)
{
  SizeType max_fid = 0;
  for ( auto fault: fault_list ) {
    max_fid = std::max(max_fid, fault.id());
  }
  ++ max_fid;
  mMarks.clear();
  mMarks.resize(max_fid, false);

  mFsim.set_fault_list(fault_list);
  mFsim.ppsfp(tv_list, [&](SizeType index, TestVector, TpgFault f) -> bool {
    mMarks[f.id()] = true;
    mFsim.set_skip(f);
    return true;
  });

  bool ok = true;
  for ( auto fault: fault_list ) {
    if ( !mMarks[fault.id()] ) {
      cout << fault.str() << " is not detected" << endl;
      ok = false;
    }
  }

  return ok;
}

END_NAMESPACE_DRUID
