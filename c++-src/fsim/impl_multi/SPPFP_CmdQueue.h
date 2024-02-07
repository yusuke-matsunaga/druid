#ifndef SPPFP_CMDQUEUE_H
#define SPPFP_CMDQUEUE_H

/// @file SPPFP_CmdQueue.h
/// @brief SPPFP_CmdQueue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SPPFP_CmdQueue SPPFP_CmdQueue.h "SPPFP_CmdQueue.h"
/// @brief sppfp 用のイベントドリブンシミュレーション用のコマンドキュー
///
/// - 複数のスレッドから参照されるので排他制御が必要となる．
/// - メインスレッドが put() する前に子スレッドが get() すると競合が
///   発生するので子スレッドはキューにデータが溜まった状態から起動すること．
//////////////////////////////////////////////////////////////////////
class SPPFP_CmdQueue
{
public:

  /// @brief コンストラクタ
  SPPFP_CmdQueue() = default;

  /// @brief デストラクタ
  ~SPPFP_CmdQueue() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief イベントを追加する．
  void
  put(
    SizeType id,                            ///< [in] コマンド番号
    const vector<const SimNode*>& node_list ///< [in] 対象のノードのリスト
  )
  {
    std::unique_lock lck{mMutex};
    mQueue.push_back(Cmd{id, node_list});
  }

  /// @brief ENDマーカを追加する．
  void
  put_end()
  {
    std::unique_lock lck{mMutex};
    mQueue.push_back(Cmd{0, {}});
  }

  /// @brief コマンドを取り出す．
  /// @return ENDマーカの時 false を返す．
  bool
  get(
    SizeType& id,                     ///< [out] コマンド番号
    vector<const SimNode*>& node_list ///< [out] 対象のノードのリスト
  )
  {
    std::unique_lock lck{mMutex};
    ASSRET_COND ( !mQueue.empty() );
    auto& cmd = mQueue.front();
    if ( cmd.mNodeList.empty() ) {
      return false;
    }
    id = cmd.mId;
    node_list = cmd.mNodeList;
    mQueue.pop_front();
    return true;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief コマンドを表す構造体
  struct Cmd
  {
    // コマンド番号
    SizeType mId;

    // ノードのリスト
    vector<const SimNode*> mNodeList;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // mQueue 用の mutex
  std::mutex mMutex;

  // コマンドのキュー
  dequeue<Cmd> mQueue;

};

END_NAMESPACE_DRUID_FSIM

#endif // SPPFP_CMDQUEUE_H
