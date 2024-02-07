#ifndef PPSFP_THREAD_H
#define PPSFP_THREAD_H

/// @file PPSFP_Thread.h
/// @brief PPSFP_Thread のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "CmdQueue.h"
#include "EventQ.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class PPSFP_Thread PPSFP_Thread.h "PPSFP_Thread.h"
/// @brief ppsfp タイプのイベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class PPSFP_Thread
{
public:

  using cbtype = FSIM_CLASSNAME::cbtype;

public:

  /// @brief コンストラクタ
  PPSFP_Thread(
    FSIM_CLASSNAME& fsim, ///< [in] 故障シミュレータ本体
    CmdQueue& cmd_queue,  ///< [in] コマンドキュー
    SizeType pat_base,    ///< [in] パタン番号のベース値
    SizeType pat_num,     ///< [in] パタン数
    cbtype callback       ///< [in] コールバック関数
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
  CmdQueue& mCmdQueue;

  // イベントキュー
  EventQ mEventQ;

  // コールバック関数
  cbtype mCallBack;

  // パタン番号のベース値
  SizeType mPatBase;

  // パタン数
  SizeType mPatNum;

};

END_NAMESPACE_DRUID_FSIM

#endif // PPSFP_THREAD_H
