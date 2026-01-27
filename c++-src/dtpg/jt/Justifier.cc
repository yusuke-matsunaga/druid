
/// @file Justifier.cc
/// @brief Justifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Justifier.h"
#include "JustData.h"
#include "JustNaive.h"
#include "Just1.h"
#include "Just2.h"
#include "types/AssignList.h"
#include "types/TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス Justifier
//////////////////////////////////////////////////////////////////////

Justifier*
Justifier::new_obj(
  const TpgNetwork& network,
  const JsonValue& option
)
{
  if ( option.is_null() ) {
    // デフォルトフォールバックは Just2
    return new Just2(network);
  }
  if ( option.is_string() ) {
    auto just_type = option.get_string();
    if ( just_type == "naive" ) {
      return new JustNaive(network);
    }
    if ( just_type == "just1" ) {
      return new Just1(network);
    }
    if ( just_type == "just2" ) {
      return new Just2(network);
    }
    // 知らない型だった．
    std::ostringstream buf;
    buf << just_type << ": unknown value for 'justifier'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  std::ostringstream buf;
  buf << "value for 'justifier' should be a string or null";
  throw std::invalid_argument{buf.str()};
}

// @brief コンストラクタ
Justifier::Justifier(
  const TpgNetwork& network
) : mNetwork{network}
{
}

// @brief 正当化に必要な割当を求める(縮退故障用)．
AssignList
Justifier::justify(
  const AssignList& assign_list,
  const VidMap& var_map,
  const SatModel& model
)
{
  JustData jd{var_map, model};
  mJustDataPtr = &jd;
  return _justify(assign_list);
}

// @brief 正当化に必要な割当を求める(遷移故障用)．
AssignList
Justifier::justify(
  const AssignList& assign_list,
  const VidMap& var1_map,
  const VidMap& var2_map,
  const SatModel& model
)
{
  JustData jd{var1_map, var2_map, model};
  mJustDataPtr = &jd;
  return _justify(assign_list);
}

END_NAMESPACE_DRUID
