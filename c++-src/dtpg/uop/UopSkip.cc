
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
  const TpgFault* f
)
{
  auto fid = f->id();
  if ( mUntestCountMap.count(fid) == 0 ) {
    // はじめて検出不能になった．
    mUntestCountMap.emplace(fid, 0);
    mUntestList.push_back(fid);
  }

  // 検出不能回数を1増やす．
  ++ mUntestCountMap[fid];

  if ( mUntestCountMap[fid] >= mThreshold ) {
    // 検出不能回数がしきい値を越えたのでスキップする．
    mSkipList.push_back(fid);
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
