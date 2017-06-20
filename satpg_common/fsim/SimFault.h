﻿#ifndef SIMFAULT_H
#define SIMFAULT_H

/// @file SimFault.h
/// @brief SimFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class SimFault SimFault.h "SimFault.h"
/// @brief 故障シミュレーション用の故障関係のデータ構造
//////////////////////////////////////////////////////////////////////
struct SimFault
{
  /// @brief 内容を設定する便利関数
  /// @param[in] f オリジナルの故障
  /// @param[in] node 対応する SimNode
  /// @param[in] ipos 入力番号
  /// @param[in] inode 入力に対応する SimNode
  /// @note ipos と inode は f が入力の故障の時のみ意味を持つ．
  void
  set(const TpgFault* f,
      SimNode* node,
      ymuint ipos,
      SimNode* inode)
  {
    mOrigF = f;
    mNode = node;
    mIpos = ipos;
    mInode = inode;
    mSkip = true;
  }

  // 元の故障
  const TpgFault* mOrigF;

  // 故障のあるゲート
  SimNode* mNode;

  // 入力の故障の場合の入力位置
  ymuint mIpos;

  // 入力の故障の場合の入力のゲート
  SimNode* mInode;

  // 現在計算中のローカルな故障伝搬マスク
  PackedVal mObsMask;

  // スキップフラグ
  bool mSkip;

};

END_NAMESPACE_YM_SATPG_FSIM

#endif // SIMFAULT_H
