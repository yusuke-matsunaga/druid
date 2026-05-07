
/// @file SimThrFunc.cc
/// @brief SimThrFunc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimThrFunc.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimThrFunc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimThrFunc::SimThrFunc(
  SizeType id,
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const std::vector<SizeType>& ffr_list
) : mId{id},
    mEngine(network, fault_list, ffr_list)
{
}

// @brief デストラクタ
SimThrFunc::~SimThrFunc()
{
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp()
{
  mDetListArray.clear();
  mDetListArray.resize(1);
  mDetListArray[0] = mEngine.sppfp();
}

void
SimThrFunc::ppsfp(
  SizeType tv_num
)
{
  mDetListArray = mEngine.ppsfp(tv_num);
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp2()
{
  mResArray.clear();
  mResArray.resize(1);
  mResArray[0] = mEngine.sppfp2();
}

void
SimThrFunc::ppsfp2(
  SizeType tv_num
)
{
  mResArray = mEngine.ppsfp2(tv_num);
}

END_NAMESPACE_DRUID_FSIM
