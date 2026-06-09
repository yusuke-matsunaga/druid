
/// @file CandMgr.cc
/// @brief CandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"
#include "NaiveCandMgr.h"
#include "DichoCandMgr.h"
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
  CandMgr* mgr = nullptr;
  if ( str == "naive" ) {
    mgr = new NaiveCandMgr(fault_list);
  }
  else if ( str == "dichotomy" ) {
    mgr = new DichoCandMgr(fault_list);
  }
  else if ( str == "dichotomy2" ) {
    mgr = new DichoCandMgr2(fault_list);
  }
  else {
    std::ostringstream buf;
    buf << str << ": unknown option for 'method'";
    throw std::invalid_argument{buf.str()};
  }
  return std::unique_ptr<CandMgr>{mgr};
}

END_NAMESPACE_DRUID
