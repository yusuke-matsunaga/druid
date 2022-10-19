
/// @file Rtpg.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "Rtpg.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Rtpg
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Rtpg::Rtpg(
  const TpgNetwork& network,
  TvMgr& tvmgr,
  FaultType fault_type
) : mFaultType{fault_type},
    mTvMgr{tvmgr}
{
  mFsim = Fsim::new_Fsim2(network, fault_type);

  for ( int i: Range(0, PV_BITLEN) ) {
    tv_array[i] = mTvMgr.new_vector();
  }

  mDetFaultList.clear();
  mPatternList.clear();
}

// @brief デストラクタ
Rtpg::~Rtpg()
{
  delete mFsim;

  for ( int i: Range(0, PV_BITLEN) ) {
    mTvMgr.delete_vector(tv_array[i]);
  }
}

// @brief 乱数生成器を初期化する．
void
Rtpg::randgen_init(
  ymuint32 seed
)
{
  mRandGen.init(seed);
}

// @brief 1セット(PV_BITLEN個)のパタンで故障シミュレーションを行う．
// @return 新たに検出された故障数を返す．
SizeType
Rtpg::do_fsim()
{
}

// @brief 検出された故障のリストを返す．
const vector<const TpgFault*>&
Rtpg::det_fault_list() const
{
  return mDetFaultList;
}

// @brief 故障を検出したパタンのリストを返す．
const vector<const TestVector*>&
Rtpg::pattern_list() const
{
  return mPatternList;
}

END_NAMESPACE_DRUID
