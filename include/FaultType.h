#ifndef FAULTTYPE_H
#define FAULTTYPE_H

/// @file FaultType.h
/// @brief FaultType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 故障の種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum class FaultType {
  /// @brief 不正な値
  None,
  /// @brief 縮退故障
  StuckAt,
  /// @brief 遷移故障
  TransitionDelay,
};

/// @brief FaultType のストリーム出力演算子
inline
ostream&
operator<<(ostream& s,
	   FaultType ftype)
{
  switch ( ftype ) {
  case FaultType::None:             s << "NONE"; break;
  case FaultType::StuckAt :         s << "stuck-at fault"; break;
  case FaultType::TransitionDelay : s << "transition-delay fault"; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
// cython とのインターフェイスのために暫定的に用意した関数
//////////////////////////////////////////////////////////////////////

/// @brief FaultType を int に変換する．
inline
int
__fault_type_to_int(FaultType fault_type)
{
  switch ( fault_type ) {
  case FaultType::None:             return 0;
  case FaultType::StuckAt :         return 1;
  case FaultType::TransitionDelay : return 2;
  }
}

/// @brief int を FaultType に変換する．
inline
FaultType
__int_to_fault_type(int fval)
{
  switch ( fval ) {
  case 0: return FaultType::None;
  case 1: return FaultType::StuckAt;
  case 2: return FaultType::TransitionDelay;
  }
  ASSERT_NOT_REACHED;
  return FaultType::None;
}

END_NAMESPACE_DRUID

#endif // FAULTTYPE_H
