#ifndef RESQUEUE_H
#define RESQUEUE_H

/// @file ResQueue.h
/// @brief ResQueue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class ResQueue ResQueue.h "ResQueue.h"
/// @brief 結果を格納するキュー
///
/// - 子スレッドが put() する前にメインスレッドが get() すると競合が
///   発生するので子スレッドの終了を待ってから get() する必要がある．
//////////////////////////////////////////////////////////////////////
class ResQueue
{
public:

  /// @brief コンストラクタ
  ResQueue() = default;

  /// @brief デストラクタ
  ~ResQueue() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を追加する．
  void
  put(
    SizeType id,                       ///< [in] コマンド番号
    const vector<PackedVal>& obs_array ///< [in] 故障伝搬ビットの配列
  )
  {
    std::unique_lock lck{mMutex};
    mQueue.push_back(Result{id, obs_array});
  }

  /// @brief 結果を取り出す．
  bool
  get(
    SizeType& id,                ///< [out] コマンド番号
    vector<PackedVal>& obs_array ///< [out] 故障伝搬ビットの配列
  )
  {
    std::unique_lock lck{mMutex};
    if ( mQueue.empty() ) {
      return false;
    }
    auto& res = mQueue.front();
    id = res.mId;
    obs_array = res.mObsArray;
    mQueue.pop_front();
    return true;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を表す構造体
  struct Result
  {
    SizeType mId;                ///< コマンド番号
    vector<PackedVal> mObsArray; ///< 故障伝搬ビットのリスト
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // mQueue 用の mutex
  std::mutex mMutex;

  // 結果のキュー
  std::deque<Result> mQueue;

};

END_NAMESPACE_DRUID_FSIM

#endif // RESQUEUE_H
