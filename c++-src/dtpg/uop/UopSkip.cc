
/// @file UopSkip.h
/// @brief UopSkip の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UopSkip.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief 'skip' タイプを生成する．
UntestOp*
new_UopSkip(
  SizeType threshold
)
{
  return new UopSkip{threshold};
}


//////////////////////////////////////////////////////////////////////
// クラス UopSkip
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
UopSkip::UopSkip(
  SizeType threshold
) : mThreshold{threshold}
{
}

// @brief デストラクタ
UopSkip::~UopSkip()
{
  clear();
}

// @brief テスト不能故障と判定された時の処理
void
UopSkip::operator()(
  const TpgFault& f
)
{
  if ( mUntestCountMap.count(f.id()) == 0 ) {
    // はじめて検出不能になった．
    mUntestCountMap.emplace(f.id(), 0);
    mUntestList.push_back(f.id());
  }

  // 検出不能回数を1増やす．
  ++ mUntestCountMap[f.id()];

  if ( mUntestCountMap[f.id()] >= mThreshold ) {
    // 検出不能回数がしきい値を越えたのでスキップする．
    mSkipList.push_back(f.id());
  }
}

// @brief 検出不能回数とスキップフラグをクリアする．
void
UopSkip::clear()
{
  mUntestCountMap.clear();
  mUntestList.clear();
  mSkipList.clear();
}

END_NAMESPACE_DRUID
