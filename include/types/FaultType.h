#ifndef FAULTTYPE_H
#define FAULTTYPE_H

/// @file FaultType.h
/// @brief FaultType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @brief 故障の種類を表す列挙型
/// @ingroup TypesGroup
//////////////////////////////////////////////////////////////////////
enum class FaultType : std::uint8_t {
  None,            ///< 不正な値
  StuckAt,         ///< 縮退故障
  TransitionDelay, ///< 遷移故障
  GateExhaustive   ///< ゲート網羅故障
};

/// @brief FaultType のストリーム出力演算子
inline
std::ostream&
operator<<(
  std::ostream& s,
  FaultType ftype
)
{
  switch ( ftype ) {
  case FaultType::None:            s << "NONE"; break;
  case FaultType::StuckAt:         s << "stuck-at fault"; break;
  case FaultType::TransitionDelay: s << "transition-delay fault"; break;
  case FaultType::GateExhaustive:  s << "gate-exhaustive fault"; break;
  }
  return s;
}

END_NAMESPACE_DRUID

#endif // FAULTTYPE_H
