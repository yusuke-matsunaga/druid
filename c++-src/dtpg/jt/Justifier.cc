
/// @file Justifier.cc
/// @brief Justifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Justifier.h"
#include "Just1.h"
#include "Just2.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

JustImpl*
new_just(
  const JsonValue& option,
  SizeType max_id
)
{
  if ( option.is_null() ) {
    // デフォルトフォールバックは Just2
    return new Just2(max_id);
  }
  if ( option.is_string() ) {
    auto just_type = option.get_string();
    if ( just_type == "just1" ) {
      return new Just1(max_id);
    }
    if ( just_type == "just2" ) {
      return new Just2(max_id);
    }
    // 知らない型だった．
    ostringstream buf;
    buf << just_type << ": unknown value for 'justifier'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  ostringstream buf;
  buf << "value for 'justifier' should be a string or null";
  throw std::invalid_argument{buf.str()};
  // ダミー
  return nullptr;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス Justifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Justifier::Justifier(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mImpl{new_just(option, network.node_num())}
{
}

// @brief デストラクタ
Justifier::~Justifier()
{
  // JustImpl のサイズが必要なので
  // ヘッダファイルでは定義できない．
}

// @brief 正当化に必要な割当を求める
TestVector
Justifier::operator()(
  const NodeValList& assign_list,
  const VidMap& var1_map,
  const VidMap& var2_map,
  const SatModel& model
)
{
  bool has_prev_state = mNetwork.fault_type() == FaultType::TransitionDelay;
  TestVector tv{mNetwork.input_num(), mNetwork.dff_num(), has_prev_state};
  if ( has_prev_state ) {
    auto pi_assign_list = mImpl->justify(assign_list, var1_map, var2_map, model);
    tv.set_from_assign_list(pi_assign_list);
  }
  else {
    auto pi_assign_list = mImpl->justify(assign_list, var2_map, model);
    tv.set_from_assign_list(pi_assign_list);
  }
  return tv;
}

END_NAMESPACE_DRUID
