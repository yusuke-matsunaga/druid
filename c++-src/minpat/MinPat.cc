
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
#include "MpReduce.h"
#include "MpComp.h"
#include "MpAnalyze.h"
#include "MpVerify.h"


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
  auto config = ConfigParam(option);

  bool verbose = config.get_bool_elem("verbose", false);

  // 初期解を作る．
  auto init_option = config.get_param("init");
  MpInit init(network, init_fault_list, init_option);

  // 故障を削減する．
  auto reduce_option = config.get_param("reduce");
  auto fault_list = MpReduce::run(init.det_fault_list(), reduce_option);

  auto comp_option = config.get_param("comp");
  auto tv_list = MpComp::run(init.tv_list(), fault_list, comp_option);

  if ( verbose ) {
    MpAnalyze analyzer(tv_list, fault_list);
    auto ntv = tv_list.size();
    std::vector<SizeType> pos_array(ntv);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      pos_array[i] = i;
    }
    std::sort(pos_array.begin(), pos_array.end(),
	      [&](SizeType a, SizeType b) -> bool {
		return analyzer.exclusive_num(a) > analyzer.exclusive_num(b);
	      });
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto pos = pos_array[i];
      std::cout << std::right << std::setw(5) << i
		<< ": "
		<< std::setw(6) << analyzer.exclusive_num(pos)
		<< " | "
		<< std::setw(6) << analyzer.det_num(pos)
		<< std::endl;
    }
  }

  // 結果の検証を行う．
  {
    auto verify_option = config.get_param("verify");
    auto undet_list = MpVerify::run(tv_list, init.det_fault_list(), verify_option);
    for ( auto fault: undet_list ) {
      std::cout << fault.str()
		<< " is not detected" << std::endl;
    }
    if ( !undet_list.empty() ) {
      throw std::logic_error{"tv_list is imcomplete"};
    }
  }

  return tv_list;
}

END_NAMESPACE_DRUID
