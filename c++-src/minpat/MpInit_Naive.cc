
/// @file MpInit_Naive.cc
/// @brief MpInit_Naive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpInit_Naive.h"
#include "dtpg/DtpgMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpInit_Naive
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MpInit_Naive::MpInit_Naive(
  const TpgNetwork& network
) : MpInit(network)
{
}

// @brief テストパタンのリストを求める．
std::vector<TestVector>
MpInit_Naive::run(
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  // 検出可能な故障を求める．
  auto res = DtpgMgr::run(fault_list, option);
  std::vector<TestVector> tv_list;
  tv_list.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    auto status = res.status(fault);
    if ( status == FaultStatus::Detected ) {
      add_fault(fault);
      auto tv = res.testvector(fault);
      tv_list.push_back(tv);
    }
  }
  return tv_list;
}

END_NAMESPACE_DRUID
