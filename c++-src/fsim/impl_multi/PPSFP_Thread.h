#ifndef PPSFP_THREAD_H
#define PPSFP_THREAD_H

/// @file PPSFP_Thread.h
/// @brief PPSFP_Thread のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "Fsim.h"
#include "PPSFP_CmdQueue.h"
#include "ResQueue.h"
#include "EventQ.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class PPSFP_Thread PPSFP_Thread.h "PPSFP_Thread.h"
/// @brief ppsfp タイプのイベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class PPSFP_Thread
{
public:

  using cbtype = Fsim::cbtype;

public:

  /// @brief コンストラクタ
  PPSFP_Thread(
    FSIM_CLASSNAME& fsim,      ///< [in] 故障シミュレータ本体
    PPSFP_CmdQueue& cmd_queue, ///< [in] コマンドキュー
    cbtype callback            ///< [in] コールバック関数
  );

  /// @brief デストラクタ
  ~PPSFP_Thread();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief スレッド実行の本体
  void
  operator()();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障シミュレータ
  FSIM_CLASSNAME& mFsim;

  // コマンドキュー
  PPSFP_CmdQueue& mCmdQueue;

  // コールバック関数
  cbtype mCallBack;

  // イベントキュー
  EventQ mEventQ;

  // イベントキューのタイムスタンプ
  SizeType mTimeStamp{0};

};

END_NAMESPACE_DRUID_FSIM

#endif // PPSFP_THREAD_H
