
/// @file EqGroupMgr.cc
/// @brief EqGroupMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqGroupMgr.h"
#include "EqGroupGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqGroupMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqGroupMgr>
EqGroupMgr::new_obj(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
)
{
  auto str = option.get_string_elem("method", "dichotomy");
  if ( str == "naive" ) {
    return new_naive_mgr(fault_info, fsim, option);
  }
  if ( str == "dichotomy" ) {
    return new_dichotomy_mgr(fault_info, fsim, option);
  }
#if 0
  if ( str == "dichotomy2" ) {
    return new_dichotomy_mgr2(fault_info, fsim, option);
  }
#endif

  std::ostringstream buf;
  buf << str << ": unknown option for 'method'";
  throw std::invalid_argument{buf.str()};
}

// @brief コンストラクタ
EqGroupMgr::EqGroupMgr(
  FaultInfo& fault_info,
  Fsim& fsim,
  const ConfigParam& option
) : RedMgr(fault_info, fsim)
{
}

// @brief 故障シミュレーションを行って故障グループを細分化する．
bool
EqGroupMgr::subdivide(
  const std::vector<TestVector>& tv_list,
  std::function<void(const FsimResults&)> callback
)
{
  std::vector<DPat> dpat_array;
  auto res = simulate(tv_list, dpat_array);
  callback(res);
  auto change = update(dpat_array);
  return change;
}

// @brief 現在の状態を取り出す．
EqGroupGraph
EqGroupMgr::cur_state() const
{
  return EqGroupGraph(*this);
}

END_NAMESPACE_DRUID
