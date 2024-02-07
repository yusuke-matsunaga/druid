#ifndef SPPFP_THREAD_H
#define SPPFP_THREAD_H

/// @file SPPFP_Thread.h
/// @brief SPPFP_Thread のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "SPPFP_CmdQueue.h"
#include "ResQueue.h"
#include "EventQ.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SPPFP_Thread SPPFP_Thread.h "SPPFP_Thread.h"
/// @brief イベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class SPPFP_Thread
{
public:

  /// @brief コンストラクタ
  SPPFP_Thread(
    const SimContext& context, ///< [in] シミュレーションのコンテキスト
    SPPFP_CmdQueue& cmd_queue, ///< [in] コマンドキュー
    ResQueue& res_queue        ///< [in] 結果キュー
  );

  /// @brief デストラクタ
  ~SPPFP_Thread();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief スレッド実行の本体
  void
  operator()();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 値の同期を行う．
  sync_vals();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // シミュレーションのコンテキスト
  SimContext mContext;

  // コマンドキュー
  SPPFP_CmdQueue& mCmdQueue;

  // イベントキュー
  EventQ mEventQ;

  // イベントキューのタイムスタンプ
  SizeType mTimeStamp{0};

  // 結果キュー
  ResQueue mResQueue;

};

END_NAMESPACE_DRUID_FSIM

#endif // SPPFP_THREAD_H
