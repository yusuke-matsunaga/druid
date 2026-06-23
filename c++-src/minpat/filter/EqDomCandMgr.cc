
/// @file EqDomCandMgr.cc
/// @brief EqDomCandMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "EqDomCandMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス EqDomCandMgr
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<EqDomCandMgr>
EqDomCandMgr::new_obj(
  const TpgFaultList& fault_list,
  const ConfigParam& option
)
{
  auto str = option.get_string_elem("method", "dichotomy");
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

// @brief コンストラクタ
EqDomCandMgr::EqDomCandMgr(
  const TpgFaultList& fault_list,
  const ConfigParam& option
) : mFsim(fault_list, option.get_param("fsim")),
    mFaultList{fault_list}
{
}

// @brief 故障シミュレーションを行って故障グループを細分化する．
bool
EqDomCandMgr::subdivide(
  const std::vector<TestVector>& tv_list,
  std::function<void(const FsimResults&)> callback
)
{
  mFsimTimer.start();
  auto res = mFsim.run_multi(tv_list, true);
  mFsimTimer.stop();
  callback(res);

  auto ntv = res.tv_num();
  std::vector<PackedVal> dpat_array(max_fault_size(), PV_ALL0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal bit = 1ULL << i;
    for ( auto fault: res.fault_list(i) ) {
      auto fid = fault.id();
      dpat_array[fid] |= bit;
    }
  }
  auto change = update(dpat_array);
  return change;
}

END_NAMESPACE_DRUID
