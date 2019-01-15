
/// @file UopSkip.h
/// @brief UopSkip の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "UopSkip.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief 'skip' タイプを生成する．
// @param[in] threshold しきい値
// @param[in] max_fault_id 故障番号の最大値
UntestOp*
new_UopSkip(int threshold,
	    int max_fault_id)
{
  return new UopSkip(threshold, max_fault_id);
}


//////////////////////////////////////////////////////////////////////
// クラス UopSkip
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] threshold しきい値
// @param[in] max_fault_id 故障番号の最大値
UopSkip::UopSkip(int threshold,
		 int max_fault_id) :
  mUntestCountArray(max_fault_id, 0)
{
  mThreshold = threshold;
}

// @brief デストラクタ
UopSkip::~UopSkip()
{
  clear();
}

// @brief テスト不能故障と判定された時の処理
// @param[in] f 故障
void
UopSkip::operator()(const TpgFault* f)
{
  int& untest_count = mUntestCountArray[f->id()];
  if ( untest_count == 0 ) {
    // はじめて検出不能になった．
    mUntestList.push_back(f->id());
  }

  // 検出不能回数を1増やす．
  ++ untest_count;

  if ( untest_count >= mThreshold ) {
    // 検出不能回数がしきい値を越えたのでスキップする．
    //f->set_skip();
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
  for ( auto fid: mSkipList ) {
    //f->clear_skip();
  }
  mUntestList.clear();
  mSkipList.clear();
}

END_NAMESPACE_DRUID
