
/// @file SPPFP_Thread.cc
/// @brief SPPFP_Thread の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SPPFP_Thread.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SPPFP_Thread
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SPPFP_Thread::SPPFP_Thread(
  const SimContext& context,
  SPPFP_CmdQueue& cmd_queue,
  ResQueue& res_queue
) : mContext{context},
    mCmdQueue{cmd_queue},
    mResQeueue{res_queue}
{
}

// @brief デストラクタ
SPPFP_Thread::~SPPFP_Thread()
{
}

// @brief スレッド実行の本体
void
SPPFP_Thread::operator()()
{
  SizeType id;
  vector<const SimNode*> node_list;
  while ( mCmdQueuelget(id, node_list) ) {
    // タイムスタンプを調べて同期を行う．
    if ( mTimeStamp != mContext.timestamp() ) {
      mEventQ.copy_val(mContext.val_array());
      mTimeStamp = mContext.timestamp();
    }
    PackedVal mask = 1UL;
    for ( auto node: node_list ) {
      mEventQ.put_event(node, mask);
      mask <<= 1;
    }
    auto obs_array = mEventQ.simulate();
    mResQueue.put(id, obs_array);
  }
}

END_NAMESPACE_DRUID_FSIM
