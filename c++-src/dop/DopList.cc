
/// @file DopList.cc
/// @brief DopList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DopList
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopList::DopList()
{
}

// @brief デストラクタ
DopList::~DopList()
{
  for ( auto dop: mDopList ) {
    delete dop;
  }
}

// @brief dop を追加する．
void
DopList::add(DetectOp* dop)
{
  mDopList.push_back(dop);
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopList::operator()(const TpgFault* f,
		    const TestVector& tv)
{
  for ( auto dop_p: mDopList ) {
    DetectOp& dop = *dop_p;
    dop(f, tv);
  }
}

END_NAMESPACE_DRUID
