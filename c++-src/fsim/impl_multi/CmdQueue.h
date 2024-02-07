#ifndef CMDQUEUE_H
#define CMDQUEUE_H

/// @file CmdQueue.h
/// @brief CmdQueue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class CmdQueue CmdQueue.h "CmdQueue.h"
/// @brief ppsfp 用のスレッドで用いられる共有データ
///
/// - 具体的には次に処理する FFR 番号を持つ．
/// - 複数のスレッドから参照されるので排他制御が必要となる．
/// - このクラスは全FFR数を知らないので get() の度に番号を
///   インクリメントするだけ．
/// - 呼び出したスレッド側でオーバーフローの判定を行う必要がある．
//////////////////////////////////////////////////////////////////////
class CmdQueue
{
public:

  /// @brief コンストラクタ
  CmdQueue() = default;

  /// @brief デストラクタ
  ~CmdQueue() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次に処理すべき FFR 番号を返す．
  SizeType
  get()
  {
    std::unique_lock lck{mMutex};
    auto id = mNextId;
    ++ mNextId;
    return id;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // mQueue 用の mutex
  std::mutex mMutex;

  // 次に処理する FFR 番号
  SizeType mNextId{0};

};

END_NAMESPACE_DRUID_FSIM

#endif // CMDQUEUE_H
