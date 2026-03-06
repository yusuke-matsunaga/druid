
/// @file MinPat.cc
/// @brief MinPat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MinPat.h"
#include "dtpg/DtpgMgr.h"
#include "fsim/Fsim.h"
#include "types/TestVector.h"
#include "MpInit.h"
#include "MpComp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MinPat
//////////////////////////////////////////////////////////////////////

// @brief パタン圧縮を行う．
std::vector<TestVector>
MinPat::run(
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  auto init_type = get_string(option, "init", "naive");
  std::unique_ptr<MpInit> init = MpInit::new_obj(init_type, network);

  auto tv_list = init->run(fault_list, option);
  auto det_fault_list = init->fault_list();

  auto comp_type = get_string(option, "comp", "simple");
  std::unique_ptr<MpComp> comp = MpComp::new_obj(comp_type);
  auto tv_list2 = comp->run(tv_list, det_fault_list, option);

  return tv_list2;
}

END_NAMESPACE_DRUID
