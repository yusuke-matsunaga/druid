
/// @file DopDummy.cc
/// @brief DopDummy の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DopDummy.h"


BEGIN_NAMESPACE_DRUID

// @brief 'dummy' タイプを生成する．
DetectOp*
new_DopDummy()
{
  return new DopDummy;
}


//////////////////////////////////////////////////////////////////////
// クラス DopDummy
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopDummy::DopDummy()
{
}

// @brief デストラクタ
DopDummy::~DopDummy()
{
}

// @brief テストパタンが見つかった時の処理
void
DopDummy::operator()(
  const TpgFault& f,
  const TestVector& tv
)
{
}

END_NAMESPACE_DRUID
