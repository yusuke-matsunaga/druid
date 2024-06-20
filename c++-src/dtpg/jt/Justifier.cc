
/// @file Justifier.cc
/// @brief Justifier の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Justifier.h"
#include "Just1.h"
#include "Just2.h"
#include "NodeTimeValList.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

JustImpl*
new_just(
  const TpgNetwork& network,
  const JsonValue& option
)
{
  SizeType max_id = network.node_num();
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
) : mHasPrevState{network.has_prev_state()},
    mImpl{new_just(network, option)}
{
}

// @brief デストラクタ
Justifier::~Justifier()
{
  // JustImpl のサイズが必要なので
  // ヘッダファイルでは定義できない．
}

// @brief 正当化に必要な割当を求める
NodeTimeValList
Justifier::operator()(
  const NodeTimeValList& assign_list,
  const VidMap& var1_map,
  const VidMap& var2_map,
  const SatModel& model
)
{
  if ( mHasPrevState ) {
    return mImpl->justify(assign_list, var1_map, var2_map, model);
  }
  else {
    return mImpl->justify(assign_list, var2_map, model);
  }
}

END_NAMESPACE_DRUID
