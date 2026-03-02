
/// @file minpat.cc
/// @brief minpat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "MinPat.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "dtpg/DtpgMgr.h"
#include "fsim/Fsim.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

int
minpat(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    std::cout << "Usage: minpat <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];

  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto dtpg_option = JsonValue::object();
  dtpg_option.add("group_mode", JsonValue("ffr_mt"));
  auto fault_list = network.rep_fault_list();
  auto results = DtpgMgr::run(fault_list, dtpg_option);
  std::vector<TpgFault> det_list;
  det_list.reserve(fault_list.size());
  std::vector<AssignList> pat_list;
  pat_list.reserve(fault_list.size());

  auto fsim_option = JsonValue::object();
  fsim_option.add("has_x", JsonValue(true));
  Fsim fsim(network, fault_list, fsim_option);

  bool ng = false;
  for ( auto fault: fault_list ) {
    auto status = results.status(fault);
    if ( status != FaultStatus::Detected ) {
      continue;
    }
    det_list.push_back(fault);
    pat_list.push_back(results.assign_list(fault));
    auto tv = results.testvector(fault);
    { // 検証
      DiffBits _;
      auto res = fsim.spsfp(tv, fault, _);
      if ( !res ) {
	std::cout << "Error! " << fault.str() << " is not detected" << std::endl;
	ng = true;
      }
    }
  }
  if ( ng ) {
    return 1;
  }
  std::cout << "DTPG end" << std::endl
	    << "# of detected faults: " << det_list.size() << std::endl
	    << "# of initial patterns: " << pat_list.size() << std::endl;

  MinPat minpat(network, TpgFaultList(det_list), pat_list);
  auto tv_list = minpat.run();
  std::cout << "# of final patterns: " << tv_list.size() << std::endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::minpat(argc, argv);
}
