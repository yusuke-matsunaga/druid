
/// @file JustImpl.cc
/// @brief JustImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "JustImpl.h"
#include "JustData.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス JustImpl
//////////////////////////////////////////////////////////////////////

// @brief 正当化に必要な割当を求める(縮退故障用)．
NodeTimeValList
JustImpl::justify(
  const NodeTimeValList& assign_list,
  const VidMap& var_map,
  const SatModel& model
)
{
  JustData jd{var_map, model};
  mJustDataPtr = &jd;
  return _justify(assign_list);
}

// @brief 正当化に必要な割当を求める(遷移故障用)．
NodeTimeValList
JustImpl::justify(
  const NodeTimeValList& assign_list,
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
