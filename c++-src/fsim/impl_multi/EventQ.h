#ifndef FSIM_EVENTQ_H
#define FSIM_EVENTQ_H

/// @file EventQ.h
/// @brief EventQ のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "SimNode.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class EventQ EventQ.h "EventQ.h"
/// @brief 故障シミュレーション用のイベントキュー
///
/// キューに詰まれる要素は EvNode で，各々のノードはレベルを持つ．
/// このキューではレベルの小さい順に処理してゆく．同じレベルのノード
/// 間の順序は任意でよい．
//////////////////////////////////////////////////////////////////////
class EventQ
{
private:

  /// @brief イベントを表す構造体
  struct Event
  {
    /// @brief 対象の SimNode
    const SimNode* mSimNode;

    /// @brief イベントキューの次の要素
    Event* mLink{nullptr};
  };


public:

  /// @brief コンストラクタ
  EventQ(
    SizeType max_level,  ///< [in] 最大レベル
    SizeType node_num    ///< [in] ノード数
  ) :mArray(max_level, nullptr),
     mEventMap(node_num, nullptr)
  {
  }

  /// @brief デストラクタ
  ~EventQ() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンアウトのノードをキューに積む．
  void
  put_fanouts(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    auto no = node->fanout_num();
    if ( no == 1 ) {
      put(node->fanout_top());
    }
    else {
      for ( auto i: Range(0, no) ) {
	put(node->fanout(i));
      }
    }
  }

  /// @brief キューに積む
  void
  put(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    if ( mEventMap[node->id()] == nullptr ) {
      auto ev = new Event{node, nullptr};
      ++ mNum;
      mEventMap[node->id()] = ev;
      auto level = node->level();
      auto& w = mArray[level];
      ev->mLink = w;
      w = ev;
      if ( mNum == 1 || mCurLevel > level ) {
	mCurLevel = level;
      }
    }
  }

  /// @brief キューから取り出す．
  /// @retval nullptr キューが空だった．
  const SimNode*
  get()
  {
    if ( mNum > 0 ) {
      // mNum が正しければ mCurLevel がオーバーフローすることはない．
      for ( ; ; ++ mCurLevel ) {
	auto& w = mArray[mCurLevel];
	auto ev = w;
	if ( ev != nullptr ) {
	  auto node = ev->mSimNode;
	  mEventMap[node->id()] = nullptr;
	  w = ev->mLink;
	  delete ev;
	  -- mNum;
	  return node;
	}
      }
    }
    return nullptr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // レベルごとのキューの先頭ノードの配列
  std::vector<Event*> mArray;

  // 現在のレベル．
  SizeType mCurLevel{0};

  // キューに入っているノード数
  SizeType mNum{0};

  // ノード番号をキーにして Event を保持する配列
  // キューに入っていない場合には nullptr を持つ．
  std::vector<Event*> mEventMap;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_EVENTQ_H
