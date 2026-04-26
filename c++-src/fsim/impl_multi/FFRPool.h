#ifndef FFRPOOL_H
#define FFRPOOL_H

/// @file FFRPool.h
/// @brief FFRPool のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class FFRPool FFRPool.h "FFRPool.h"
/// @brief FFR 番号を持つクラス
///
/// マルチスレッド実行での排他制御を行う．
//////////////////////////////////////////////////////////////////////
class FFRPool
{
public:

  /// @brief コンストラクタ
  FFRPool(
    const std::vector<const SimFFR*>& ffr_list ///< [in] FFR のリスト
  ) : mFFRList{ffr_list},
      mNextIndex{0}
  {
  }

  /// @brief デストラクタ
  ~FFRPool() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR を一つ取り出す．
  ///
  /// なければ nullptr を返す．
  const SimFFR*
  ffr()
  {
    std::unique_lock lck{mMtx};
    if ( mNextIndex >= mFFRList.size() ) {
      return nullptr;
    }
    auto ans = mFFRList[mNextIndex];
    ++ mNextIndex;
    return ans;
  }

  /// @brief FFR のリストを取り出す．
  ///
  /// なければ空のリストを返す．
  std::vector<const SimFFR*>
  ffr_list(
    SizeType max_size ///< [in] 最大の要素数
  )
  {
    auto n = mFFRList.size();
    SizeType from;
    SizeType to;
    {
      // クリティカルな部分は mNextIndex にアクセスする部分
      std::unique_lock lck{mMtx};
      if ( mNextIndex >= n ) {
	return {};
      }
      from = mNextIndex;
      to = std::min(n, mNextIndex + max_size);
      mNextIndex = to;
    }
    std::vector<const SimFFR*> ans_list;
    ans_list.reserve(max_size);
    for ( SizeType index = from; index < to; ++ index ) {
      auto ffr = mFFRList[index];
      ans_list.push_back(ffr);
    }
    return ans_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR のリスト
  std::vector<const SimFFR*> mFFRList;

  // mFFRList 中の次のインデックス
  SizeType mNextIndex;

  // ミューテックス
  std::mutex mMtx;

};

END_NAMESPACE_DRUID_FSIM

#endif // FFRPOOL_H
