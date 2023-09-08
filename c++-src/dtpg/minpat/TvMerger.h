#ifndef TVMERGER_H
#define TVMERGER_H

/// @file TvMerger.h
/// @brief TvMerger のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
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
  TvMerger(
    const vector<TestVector>& tv_list ///< [in] 元のテストベクタのリスト
  );

  /// @brief デストラクタ
  ~TvMerger();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 極大両立集合のリストを求める．
  vector<TestVector>
  gen_mcset();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 極大集合を求める．
  void
  greedy_mcset(
    vector<int>& signature, ///< [inout] シグネチャ
    int count
  );

  /// @brief シグネチャから最も価値の低いビットを選ぶ．
  int
  select_bit(
    const vector<int>& signature ///< [in] シグネチャ
  );

  /// @brief シグネチャからテストベクタを作る．
  TestVector
  gen_vector(
    const vector<int>& signature ///< [in] シグネチャ
  );

  /// @brief テストベクタとシグネチャが両立しているか調べる．
  bool
  check_compatible(
    const TestVector& tv,        ///< [in] テストベクタ
    const vector<int>& signature ///< [in] シグネチャ
  );

  /// @brief ブロックリストを得る．
  const vector<int>&
  block_list(
    int bit, ///< [in] ビット位置
    int val  ///< [in] 値 ( 0 or 1 )
  ) const
  {
    ASSERT_COND( bit >= 0 && bit < mBitLen );

    return mBlockListArray[bit * 2 + val];
  }

  /// @brief ブロックリストを得る．
  vector<int>&
  _block_list(
    int bit, ///< [in] ビット位置
    int val  ///< [in] 値 ( 0 or 1 )
  )
  {
    ASSERT_COND( bit >= 0 && bit < mBitLen );

    return mBlockListArray[bit * 2 + val];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 元のテストベクタのリスト
  vector<TestVector> mOrigTvList;

  // ビット長
  SizeType mBitLen;

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

END_NAMESPACE_DRUID

#endif // TVMERGER_H
