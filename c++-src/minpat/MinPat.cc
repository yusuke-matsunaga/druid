
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
#include "MpReducer.h"
#include "MpComp.h"
#include "PatAnalyzer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MinPat
//////////////////////////////////////////////////////////////////////

// @brief パタン圧縮を行う．
std::vector<TestVector>
MinPat::run(
  const TpgNetwork& network,
  const TpgFaultList& init_fault_list,
  const JsonValue& option
)
{
  // 初期解を作る．
  JsonValue init_option;
  if ( option.is_object() && option.has_key("init") ) {
    init_option = option.at("init");
  }
  MpInit init(network, init_fault_list, init_option);

  // 故障を削減する．
  JsonValue reduce_option;
  if ( option.is_object() && option.has_key("reduce") ) {
    reduce_option = option.at("reduce");
  }
  auto fault_list = MpReducer::run(init.det_fault_list(), reduce_option);

  auto tv_list = init.tv_list();

  auto comp_type = get_string(option, "comp", "simple");
  std::unique_ptr<MpComp> comp = MpComp::new_obj(comp_type);
  auto tv_list2 = comp->run(tv_list, fault_list, option);

  {
    PatAnalyzer analyzer(tv_list2, fault_list);
    auto ntv = tv_list2.size();
    std::vector<SizeType> ex_num_array;
    ex_num_array.reserve(ntv);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto n = analyzer.exclusive_list(i).size();
      ex_num_array.push_back(n);
    }
    std::sort(ex_num_array.begin(), ex_num_array.end(), std::greater<>());
    for ( SizeType i = 0; i < tv_list2.size(); ++ i ) {
      std::cout << std::setw(5) << i
		<< ": "
		<< std::setw(6) << ex_num_array[i] << std::endl;
    }
  }

  // 結果の検証を行う．
  {
    auto fsim_option = JsonValue::object();
    fsim_option.add("has_x", true);
    Fsim fsim(network, init.det_fault_list(), fsim_option);
    std::unordered_set<SizeType> det_mark;
    for ( auto& tv: tv_list2 ) {
      auto res = fsim.sppfp(tv);
      for ( auto fid: res.fault_list(0) ) {
	det_mark.insert(fid);
	auto fault = network.fault(fid);
	fsim.set_skip(fault);
      }
    }
    bool ng = false;
    for ( auto fault: init.det_fault_list() ) {
      if ( det_mark.count(fault.id()) == 0 ) {
	std::cout << fault.str()
		  << " is not detected" << std::endl;
	ng = true;
      }
    }
    if ( ng ) {
      throw std::logic_error{"new_tv_list is imcomplete"};
    }
  }

  return tv_list2;
}

END_NAMESPACE_DRUID
