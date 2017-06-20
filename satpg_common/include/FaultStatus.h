﻿#ifndef FAULTSTATUS_H
#define FAULTSTATUS_H

/// @file FaultStatus.h
/// @brief FaultStatus のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief 故障の状態
//////////////////////////////////////////////////////////////////////
enum FaultStatus {
  /// @brief 未検出
  kFsUndetected,
  /// @brief 検出
  kFsDetected,
  /// @brief テスト不能 (冗長)
  kFsUntestable,
  /// @brief アボート
  kFsAborted
};


/// @brief 内容を表す文字列を返す．
inline
const char*
str(FaultStatus fs)
{
  switch ( fs ) {
  case kFsUndetected: return "undetected";
  case kFsDetected:   return "detected";
  case kFsUntestable: return "untestable";
  case kFsAborted:    return "aborted";
  default: break;
  }
  ASSERT_NOT_REACHED;
  return "";
}

END_NAMESPACE_YM_SATPG

#endif // FAULTSTATUS_H
