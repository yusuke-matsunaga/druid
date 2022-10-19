
/// @file MatrixGen.cc
/// @brief MatrixGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MatrixGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
MatrixGen::MatrixGen(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  const TpgNetwork& network,
  FaultType fault_type
) : mFaultList{fault_list},
    mTvList{tv_list},
    mRowIdMap(network.max_fault_id(), -1),
    mFsim{network, fault_type, true}
{
  mFsim.clear_patterns();
  mFsim.set_skip_all();
  for ( auto row_id: Range(mFaultList.size()) ) {
    auto fault = mFaultList[row_id];
    mRowIdMap[fault->id()] = row_id;
    mFsim.clear_skip(fault);
  }
}

// @brief デストラクタ
MatrixGen::~MatrixGen()
{
}

// @brief 被覆行列を作る．
McMatrix
MatrixGen::generate()
{
  McMatrix matrix{mFaultList.size(), mTvList.size()};

  int wpos = 0;
  int tv_base = 0;
  for ( auto tv: mTvList ) {
    mFsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == PV_BITLEN ) {
      do_fsim(matrix, tv_base, wpos);
      mFsim.clear_patterns();
      wpos = 0;
      tv_base += PV_BITLEN;
    }
  }
  if ( wpos > 0 ) {
    do_fsim(matrix, tv_base, wpos);
  }

  return std::move(matrix);
}

// @brief 故障シミュレーションを行う．
void
MatrixGen::do_fsim(
  McMatrix& matrix,
  int tv_base,
  int num
)
{
  int ndet = mFsim.ppsfp();
  vector<int> det_list[PV_BITLEN];
  for ( auto i: Range(ndet) ) {
    const TpgFault* fault = mFsim.det_fault(i);
    PackedVal dbits = mFsim.det_fault_pat(i);
    int row_id = mRowIdMap[fault->id()];
    for ( auto bit: Range(num) ) {
      if ( dbits & (1UL << bit) ) {
	int tvid = tv_base + bit;
	matrix.insert_elem(row_id, tv_base + bit);
      }
    }
  }
}

END_NAMESPACE_DRUID
