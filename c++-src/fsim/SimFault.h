#ifndef SIMFAULT_H
#define SIMFAULT_H

/// @file SimFault.h
/// @brief SimFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class SimFault SimFault.h "SimFault.h"
/// @brief 故障シミュレーション用の故障関係のデータ構造
//////////////////////////////////////////////////////////////////////
class SimFault
{
public:

  /// @brief 空のコンストラクタ
  SimFault() = default;

  /// @brief コピーコンストラクタは禁止
  SimFault(const SimFault& src) = delete;

  /// @brief 代入演算子も禁止
  const SimFault&
  operator=(const SimFault& src) = delete;

  /// @brief デストラクタ
  ~SimFault() = default;


public:

  /// @brief 内容を設定する便利関数
  ///
  /// ipos と inode は f が入力の故障の時のみ意味を持つ．
  void
  set(
    const TpgFault* f, ///< [in] オリジナルの故障
    SimNode* node,     ///< [in] 対応する SimNode
    SizeType ipos,     ///< [in] 入力番号
    SimNode* inode     ///< [in] 入力に対応する SimNode
  )
  {
    mOrigF = f;
    mNode = node;
    mIpos = ipos;
    mInode = inode;
    mSkip = true;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 元の故障
  const TpgFault* mOrigF;

  // 故障のあるゲート
  SimNode* mNode;

  // 入力の故障の場合の入力位置
  SizeType mIpos;

  // 入力の故障の場合の入力のゲート
  SimNode* mInode;

  // 現在計算中のローカルな故障伝搬マスク
  PackedVal mObsMask;

  // スキップフラグ
  bool mSkip;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMFAULT_H
