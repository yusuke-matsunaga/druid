#ifndef FAULTSTATUS_H
#define FAULTSTATUS_H

/// @file FaultStatus.h
/// @brief FaultStatus のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 故障の状態
//////////////////////////////////////////////////////////////////////
enum class FaultStatus {
  /// @brief 未検出
  Undetected,
  /// @brief 検出
  Detected,
  /// @brief テスト不能 (冗長)
  Untestable
};


/// @brief 内容を表す文字列を返す．
inline
const char*
str(
  FaultStatus fault_status
)
{
  switch ( fault_status ) {
  case FaultStatus::Undetected: return "undetected";
  case FaultStatus::Detected:   return "detected";
  case FaultStatus::Untestable: return "untestable";
  default: break;
  }
  ASSERT_NOT_REACHED;
  return "";
}


//////////////////////////////////////////////////////////////////////
// Cython 用の変換関数
//////////////////////////////////////////////////////////////////////

/// @brief FaultStatus を整数に変換する
inline
int
__fault_status_to_int(
  FaultStatus status
)
{
  switch ( status ) {
  case FaultStatus::Undetected: return 0;
  case FaultStatus::Detected:   return 1;
  case FaultStatus::Untestable: return 2;
  default: ASSERT_NOT_REACHED;
  }
  return 0;
}

/// @brief 整数を FaultStatus に変換する
inline
FaultStatus
__int_to_fault_status(
  int val
)
{
  switch ( val ) {
  case 0: return FaultStatus::Undetected;
  case 1: return FaultStatus::Detected;
  case 2: return FaultStatus::Untestable;
  default: ASSERT_NOT_REACHED;
  }
  return FaultStatus::Undetected;
}

END_NAMESPACE_DRUID

#endif // FAULTSTATUS_H
