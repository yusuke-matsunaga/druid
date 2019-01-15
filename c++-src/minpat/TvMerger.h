#ifndef TVMERGER_H
#define TVMERGER_H

/// @file TvMerger.h
/// @brief TvMerger のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TvMerger TvMerger.h "TvMerger.h"
/// @brief TestVector のマージを行うクラス
//////////////////////////////////////////////////////////////////////
class TvMerger
{
public:

  /// @brief コンストラクタ
  /// @param[in] tv_list 元のテストベクタのリスト
  TvMerger(const vector<TestVector>& tv_list);

  /// @brief デストラクタ
  ~TvMerger();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 極大両立集合のリストを求める．
  /// @param[out] new_tv_list マージして生成したテストベクタのリスト
  void
  gen_mcset(vector<TestVector>& new_tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 極大集合を求める．
  /// @param[inout] signature シグネチャ
  void
  greedy_mcset(vector<int>& signature,
	       int count);

  /// @brief シグネチャから最も価値の低いビットを選ぶ．
  int
  select_bit(const vector<int>& signature);

  /// @brief シグネチャからテストベクタを作る．
  TestVector
  gen_vector(const vector<int>& signature);

  /// @brief テストベクタとシグネチャが両立しているか調べる．
  bool
  check_compatible(const TestVector& tv,
		   const vector<int>& signature);

  /// @brief ブロックリストを得る．
  /// @param[in] bit ビット位置
  /// @param[in] val 値 ( 0 or 1 )
  const vector<int>&
  block_list(int bit,
	     int val) const;

  /// @brief ブロックリストを得る．
  /// @param[in] bit ビット位置
  /// @param[in] val 値 ( 0 or 1 )
  vector<int>&
  _block_list(int bit,
	      int val);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 元のテストベクタのリスト
  vector<TestVector> mOrigTvList;

  // ビット長
  int mBitLen;

  // 各ビットのブロックリスト
  // ビット位置を b，値を v とすると
  // mBlockListArray[b * 2 + v] にこのビット位置に v と反対の
  // 値を持つテストベクタの番号(mOrigTvList上の位置)が入る．
  vector<vector<int>> mBlockListArray;

  // 0/1 の両方のブロックリストが空でないビットのリスト
  vector<int> mBitList;

  // mBlockListArray の最大値
  int mMaxNum;

  // タブーリスト
  vector<int> mTabuList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ブロックリストを得る．
// @param[in] bit ビット位置
// @param[in] val 値 ( 0 or 1 )
inline
const vector<int>&
TvMerger::block_list(int bit,
		     int val) const
{
  ASSERT_COND( bit >= 0 && bit < mBitLen );

  return mBlockListArray[bit * 2 + val];
}

// @brief ブロックリストを得る．
// @param[in] bit ビット位置
// @param[in] val 値 ( 0 or 1 )
inline
vector<int>&
TvMerger::_block_list(int bit,
		      int val)
{
  ASSERT_COND( bit >= 0 && bit < mBitLen );

  return mBlockListArray[bit * 2 + val];
}

END_NAMESPACE_DRUID

#endif // TVMERGER_H
