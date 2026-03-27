
/// @file MpComp_Simple.cc
/// @brief MpComp_Simple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp_Simple.h"
#include "MpComp_MinCov.h"
#include "MpComp_Expand.h"
#include "MpComp_Pack.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp_Simple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpComp_Simple::MpComp_Simple()
{
}

// @brief デストラクタ
MpComp_Simple::~MpComp_Simple()
{
}

// @brief パタン圧縮の本体
std::vector<TestVector>
MpComp_Simple::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  // 最小被覆を求める．
  MpComp_MinCov mincov;
  auto tv_list1 = mincov.run(tv_list, fault_list, option);

  // テストキューブの拡大を行う．
  MpComp_Expand expand;
  auto tv_list2 = expand.run(tv_list1, fault_list, option);

  // 最小彩色を行う．
  MpComp_Pack pack;
  auto tv_list3 = pack.run(tv_list2, fault_list, option);

  // 再度最小被覆を行う．
  auto tv_list4 = mincov.run(tv_list3, fault_list, option);

  return tv_list4;
}

END_NAMESPACE_DRUID
