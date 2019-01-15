
/// @file Rtpg.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Rtpg.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Rtpg
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] tvmgr TvMgr
// @param[in] fault_type 故障の種類
Rtpg::Rtpg(const TpgNetwork& network,
	   TvMgr& tvmgr,
	   FaultType fault_type) :
  mFaultType(fault_type),
  mTvMgr(tvmgr)
{
  mFsim = Fsim::new_Fsim2(network, fault_type);

  for ( int i = 0; i < kPvBitLen; ++ i ) {
    tv_array[i] = mTvMgr.new_vector();
  }

  mDetFaultList.clear();
  mPatternList.clear();
}

// @brief デストラクタ
Rtpg::~Rtpg()
{
  delete mFsim;

  for ( int i = 0; i < kPvBitLen; ++ i ) {
    mTvMgr.delete_vector(tv_array[i]);
  }
}

// @brief 乱数生成器を初期化する．
// @param[in] seed 乱数の種
void
Rtpg::randgen_init(ymuint32 seed)
{
  mRandGen.init(seed);
}

// @brief 1セット(kPvBitLen個)のパタンで故障シミュレーションを行う．
// @return 新たに検出された故障数を返す．
int
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
