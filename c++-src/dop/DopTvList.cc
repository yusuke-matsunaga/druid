
/// @file DopTvList.cc
/// @brief DopTvList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopTvList.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 'tvlist' タイプを生成する．
// @param[in] input_num 入力数
// @param[in] dff_numr DFF数
// @param[in] fault_type 故障の種類
// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvList(int input_num,
	      int dff_num,
	      FaultType fault_type,
	      vector<TestVector>& tvlist)
{
  return new DopTvList(input_num, dff_num, fault_type, tvlist);
}


//////////////////////////////////////////////////////////////////////
// クラス DopTvList
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] input_num 入力数
// @param[in] dff_numr DFF数
// @param[in] fault_type 故障の種類
// @param[in] tvlist テストベクタのリスト
DopTvList::DopTvList(int input_num,
		     int dff_num,
		     FaultType fault_type,
		     vector<TestVector>& tvlist) :
  mInputNum(input_num),
  mDffNum(dff_num),
  mFaultType(fault_type),
  mTvList(tvlist)
{
}

// @brief デストラクタ
DopTvList::~DopTvList()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopTvList::operator()(const TpgFault* f,
		      const TestVector& tv)
{
  mTvList.push_back(tv);
}

END_NAMESPACE_DRUID
