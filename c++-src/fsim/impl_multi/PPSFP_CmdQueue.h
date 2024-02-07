#ifndef PPSFP_CMDQUEUE_H
#define PPSFP_CMDQUEUE_H

/// @file PPSFP_CmdQueue.h
/// @brief PPSFP_CmdQueue のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class PPSFP_CmdQueue PPSFP_CmdQueue.h "PPSFP_CmdQueue.h"
/// @brief ppsfp 用のスレッドで用いられる共有データ
///
/// - 具体的には次に処理する FFR 番号を持つ．
/// - 複数のスレッドから参照されるので排他制御が必要となる．
/// - このクラスは全FFR数を知らないので get() の度に番号を
///   インクリメントするだけ．
/// - 呼び出したスレッド側でオーバーフローの判定を行う必要がある．
//////////////////////////////////////////////////////////////////////
class PPSFP_CmdQueue
{
public:

  /// @brief コンストラクタ
  PPSFP_CmdQueue() = default;

  /// @brief デストラクタ
  ~PPSFP_CmdQueue() = default;


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
  SizeType mNextId;

};

END_NAMESPACE_DRUID_FSIM

#endif // PPSFP_CMDQUEUE_H
