
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
  SizeType threshold,
  SizeType max_fault_id
)
{
  return new UopSkip{threshold, max_fault_id};
}


//////////////////////////////////////////////////////////////////////
// クラス UopSkip
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
UopSkip::UopSkip(
  SizeType threshold,
  SizeType max_fault_id
) : mThreshold{threshold},
    mUntestCountArray(max_fault_id, 0)
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
  SizeType& untest_count = mUntestCountArray[f->id()];
  if ( untest_count == 0 ) {
    // はじめて検出不能になった．
    mUntestList.push_back(f->id());
  }

  // 検出不能回数を1増やす．
  ++ untest_count;

  if ( untest_count >= mThreshold ) {
    // 検出不能回数がしきい値を越えたのでスキップする．
    mSkipList.push_back(f->id());
  }
}

// @brief 検出不能回数とスキップフラグをクリアする．
void
UopSkip::clear()
{
  for ( auto fid: mUntestList ) {
    mUntestCountArray[fid] = 0;
  }
  mUntestList.clear();
  mSkipList.clear();
}

END_NAMESPACE_DRUID
