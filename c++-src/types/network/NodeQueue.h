#ifndef NODEQUEUE_H
#define NODEQUEUE_H

/// @file NodeQueue.h
/// @brief NodeQueue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeQueue NodeQueue.h "NodeQueue.h"
/// @brief NodeRep のキュー
///
/// 重複追加のチェックを行う
//////////////////////////////////////////////////////////////////////
class NodeQueue
{
public:

  /// @brief コンストラクタ
  NodeQueue(
    SizeType max_id ///< [in] ノード番号の最大値 + 1
  ) : mMarkArray(max_id, false)
  {
  }

  /// @brief デストラクタ
  ~NodeQueue() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief キューが空のとき true を返す．
  bool
  empty() const
  {
    return mQueue.empty();
  }

  /// @brief キューから取り出す．
  const NodeRep*
  get()
  {
    auto node = mQueue.front();
    mQueue.pop_front();
    return node;
  }

  /// @brief キューに追加する．
  void
  put(
    const NodeRep* node
  )
  {
    if ( !mMarkArray[node->id()] ) {
      mMarkArray[node->id()] = true;
      mQueue.push_back(node);
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // キューに入っていることを示す印
  std::vector<bool> mMarkArray;

  // キュー
  std::deque<const NodeRep*> mQueue;

};

END_NAMESPACE_DRUID

#endif // NODEQUEUE_H
