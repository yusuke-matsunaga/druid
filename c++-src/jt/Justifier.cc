
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
  const string& just_type,
  SizeType max_id
)
{
  if ( just_type == "just1" ) {
    return new Just1(max_id);
  }
  if ( just_type == "just2" ) {
    return new Just2(max_id);
  }

  // デフォルトフォールバックは Just2
  return new Just2(max_id);
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス Justifier
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Justifier::Justifier(
  const string& just_type,
  const TpgNetwork& network
) : mNetwork{network},
    mImpl{new_just(just_type, network.node_num())}
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
  FaultType fault_type,
  const NodeValList& assign_list,
  const VidMap& var1_map,
  const VidMap& var2_map,
  const SatModel& model
)
{
  if ( fault_type == FaultType::StuckAt ) {
    auto pi_assign_list = mImpl->justify(assign_list, var2_map, model);
    return TestVector::new_from_assign_list(mNetwork.input_num(), mNetwork.dff_num(),
					    FaultType::StuckAt, pi_assign_list);
  }
  else if ( fault_type == FaultType::TransitionDelay ) {
    auto pi_assign_list = mImpl->justify(assign_list, var1_map, var2_map, model);
    return TestVector::new_from_assign_list(mNetwork.input_num(), mNetwork.dff_num(),
					    FaultType::TransitionDelay, pi_assign_list);
  }
  else {
    ASSERT_NOT_REACHED;
  }
  return TestVector{};
}

END_NAMESPACE_DRUID
