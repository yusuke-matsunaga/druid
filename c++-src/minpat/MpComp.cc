
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

// @brief 派生クラスのオブジェクトを生成するクラスメソッド
std::unique_ptr<MpComp>
MpComp::new_obj(
  const std::string& type
)
{
  if ( type == "simple" ) {
    return std::unique_ptr<MpComp>{new MpComp_Simple};
  }
  std::ostringstream buf;
  buf << type << ": Unknown type name";
  throw std::invalid_argument{buf.str()};
}

// @brief テストパタンの圧縮を行う．
std::vector<TestVector>
MpComp::run(
  const std::vector<TestVector>& tv_list,
  const TpgFaultList& fault_list,
  const JsonValue& option
)
{
  auto new_tv_list = _run(tv_list, fault_list, option);

  // 結果の検証を行う．
  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", true);
  auto network = fault_list.network();
  Fsim fsim(network, fault_list, fsim_option);
  std::unordered_set<SizeType> det_mark;
  for ( auto& tv: new_tv_list ) {
    auto res = fsim.sppfp(tv);
    for ( auto fid: res.fault_list(0) ) {
      det_mark.insert(fid);
      auto fault = network.fault(fid);
      fsim.set_skip(fault);
    }
  }
  bool ng = false;
  for ( auto fault: fault_list ) {
    if ( det_mark.count(fault.id()) == 0 ) {
      std::cout << fault.str()
		<< " is not detected" << std::endl;
      ng = true;
    }
  }
  if ( ng ) {
    throw std::logic_error{"new_tv_list is imcomplete"};
  }

  return new_tv_list;
}

END_NAMESPACE_DRUID
