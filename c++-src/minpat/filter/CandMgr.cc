
/// @file CandMgr.cc
/// @brief CandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "DichoCandMgr2.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス CandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<CandMgr>
CandMgr::new_obj(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto str = option.get_string_elem("method", "naive");
  if ( str == "naive" ) {
    return new_naive_mgr(fault_list, option);
  }
  if ( str == "dichotomy" ) {
    return new_dichotomy_mgr(fault_list, option);
  }
  if ( str == "dichotomy2" ) {
    return new_dichotomy_mgr2(fault_list, option);
  }

  std::ostringstream buf;
  buf << str << ": unknown option for 'method'";
  throw std::invalid_argument{buf.str()};
}

END_NAMESPACE_DRUID
