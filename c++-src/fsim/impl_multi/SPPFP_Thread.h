#ifndef SPPFP_THREAD_H
#define SPPFP_THREAD_H

/// @file SPPFP_Thread.h
/// @brief SPPFP_Thread のヘッダファイル
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
/// @class SPPFP_Thread SPPFP_Thread.h "SPPFP_Thread.h"
/// @brief イベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class SPPFP_Thread
{
public:

  using cbtype = FSIM_CLASSNAME::cbtype;

public:

  /// @brief コンストラクタ
  SPPFP_Thread(
    FSIM_CLASSNAME& fsim, ///< [in] 故障シミュレータ本体
    CmdQueue& cmd_queue,  ///< [in] コマンドキュー
    cbtype callback       ///< [in] コールバック関数
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

  /// @brief 実際にイベントドリヴンシミュレーションを行う．
  void
  do_simulation();


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

  // FFR のバッファ
  const SimFFR* mFFRArray[PV_BITLEN];

  // mFFRArray の次の挿入位置
  SizeType mFFRPos;

};

END_NAMESPACE_DRUID_FSIM

#endif // SPPFP_THREAD_H
