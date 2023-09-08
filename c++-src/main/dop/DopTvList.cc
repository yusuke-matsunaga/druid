
/// @file DopTvList.cc
/// @brief DopTvList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DopTvList.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// @brief 'tvlist' タイプを生成する．
DetectOp*
new_DopTvList(
  vector<TestVector>& tvlist
)
{
  return new DopTvList{tvlist};
}


//////////////////////////////////////////////////////////////////////
// クラス DopTvList
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopTvList::DopTvList(
  vector<TestVector>& tvlist
) : mTvList{tvlist}
{
}

// @brief デストラクタ
DopTvList::~DopTvList()
{
}

// @brief テストパタンが見つかった時の処理
void
DopTvList::operator()(
  const TpgFault& f,
  const TestVector& tv
)
{
  mTvList.push_back(tv);
}

END_NAMESPACE_DRUID
