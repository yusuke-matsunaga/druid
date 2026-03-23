
/// @file MpComp.cc
/// @brief MpComp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MpComp.h"
#include "MpComp_Simple.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "fsim/Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MpComp
//////////////////////////////////////////////////////////////////////

// @brief テストパタンの圧縮を行う．
std::vector<TestVector>
MpComp::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  std::cout << std::left << std::setw(20)
	    << "Compaction start:" << std::endl
	    << std::left << std::setw(20)
	    << "# of faults: " << fault_list.size() << std::endl
	    << std::left << std::setw(20)
	    << "# of initial pats: " << tv_list.size() << std::endl;

  auto obj = MpCompImpl::new_obj(option);
  auto new_tv_list = obj->_run(tv_list, fault_list, option);
  return new_tv_list;
}

END_NAMESPACE_DRUID
