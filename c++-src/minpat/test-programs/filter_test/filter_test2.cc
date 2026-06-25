
/// @file filter_test.cc
/// @brief DiFilter/NaiveFilter のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "PatGen.h"
#include "EqDomMgr.h"
#include "EqDomCand.h"
#include "FaultAnalyze.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"
#include "ym/Timer.h"
#include "ym/JsonValue.h"
#include <unistd.h> // getopt
#include <libgen.h> // basename


BEGIN_NAMESPACE_DRUID

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " [--blif|--iscas89] <file>"
	    << std::endl;
}

BEGIN_NONAMESPACE

std::string
time_str(
  Timer& timer
)
{
  std::ostringstream buf;
  buf << std::setw(11)
      << std::fixed
      << std::setprecision(2)
      << timer.get_time()
      << "ms";
  return buf.str();
}

END_NONAMESPACE

int
filter_test(
  int argc,
  char** argv
)
{
  std::string format = "blif";
  FaultType ftype = FaultType::StuckAt;
  bool ffr_reduction = false;
  bool mffc_reduction = false;
  int no_change_limit = 0;
  int batch_size = 0;
  int sat_time_limit = 0;
  std::string dicho_str = "dichotomy";
  bool multi_thread = false;
  bool verbose = false;
  int debug = 0;
  bool dump = false;

  SizeType argpos = 1;
  for ( ; argpos < argc; ++ argpos ) {
    if ( argv[argpos][0] != '-' ) {
      break;
    }
    std::string arg = argv[argpos];
    if ( arg == "--blif" ) {
      format = "blif";
    }
    else if ( arg == "--iscas89" ) {
      format = "iscas89";
    }
    else if ( arg == "--stuck-at" ) {
      ftype = FaultType::StuckAt;
    }
    else if ( arg == "--transition-delay" ) {
      ftype = FaultType::TransitionDelay;
    }
    else if ( arg == "--ffr-reduction" ) {
      ffr_reduction = true;
    }
    else if ( arg == "--mffc-reduction" ) {
      mffc_reduction = true;
    }
    else if ( arg == "--no-change-limit" ) {
      ++ argpos;
      if ( argpos >= argc ) {
	std::cerr << "'--no-change-limit' requires <int> value";
	return 2;
      }
      std::string val = argv[argpos];
      no_change_limit = stoi(val);
    }
    else if ( arg == "--batch-size" ) {
      ++ argpos;
      if ( argpos >= argc ) {
	std::cerr << "'--batch-size' requires <int> value";
	return 2;
      }
      std::string val = argv[argpos];
      batch_size = stoi(val);
    }
    else if ( arg == "--dichotomy2" ) {
      dicho_str = "dichotomy2";
    }
    else if ( arg == "--multi-thread" ) {
      multi_thread = true;
    }
    else if ( arg == "--sat-time-limit" ) {
      ++ argpos;
      if ( argpos >= argc ) {
	std::cerr << "'--sat-time-limit' requires <int> value";
	return 2;
      }
      std::string val_str = argv[argpos];
      sat_time_limit = stoi(val_str);
    }
    else if ( arg == "--verbose" ) {
      verbose = true;
    }
    else if ( arg == "--debug" ) {
      ++ debug;
    }
    else if ( arg == "--dump" ) {
      dump = true;
    }
    else {
      std::cerr << arg << ": Unknown option" << std::endl;
      usage();
      return 1;
    }
  }
  if ( argpos != argc - 1 ) {
    usage();
    std::cout << "Usage: minpat <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[argpos];

  TpgNetwork network;
  try {
    network = TpgNetwork::read_network(filename, format, ftype);
  }
  catch ( std::invalid_argument error ) {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  if ( dump ) {
    network.print(std::cout);
  }

  auto global_option = JsonValue::object();
  global_option.add("multi_thread", multi_thread);
  global_option.add("verbose", verbose);
  global_option.add("debug", debug);

  auto option = JsonValue::object();
  option.add("*", global_option);
  {
    auto analyze_option = JsonValue::object();
    analyze_option.add("ffr_reduction", ffr_reduction);
    analyze_option.add("mffc_reduction", mffc_reduction);
    analyze_option.add("global_reduction", false);
    if ( no_change_limit > 0 ) {
      analyze_option.add("no_change_limit", no_change_limit);
    }
    if ( batch_size > 0 ) {
      analyze_option.add("batch_size", batch_size);
    }
    if ( sat_time_limit > 0 ) {
      analyze_option.add("time_limit", sat_time_limit);
    }
    option.add("analyze", analyze_option);
  }

  auto original_fault_list = network.rep_fault_list();

  FaultInfo fault_info;
  {
    auto analyze_option = ConfigParam(option).get_param("analyze");
    fault_info = FaultAnalyze::run(original_fault_list, analyze_option);
    auto rep_fault_list = fault_info.rep_fault_list();
    std::cout << "# of initial faults: " << rep_fault_list.size() << std::endl;
  }

  // Dichotomy method
  std::cout << std::endl;
  std::cout << dicho_str << std::endl;
  auto dicho_option = option;
  {
    auto filter_option = JsonValue::object();
    if ( no_change_limit > 0 ) {
      filter_option.add("no_change_limit", no_change_limit);
    }
    if ( batch_size > 0 ) {
      filter_option.add("batch_size", batch_size);
    }
    auto cand_option = JsonValue::object();
    cand_option.add("method", dicho_str);
    filter_option.add("candmgr", cand_option);
    dicho_option.add("filter", filter_option);
  }

  // パラメータの取得
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));

  Timer timer;
  timer.start();

  auto fault_list = fault_info.rep_fault_list();

  // 故障シミュレータ
  auto fsim_option = ConfigParam(option).get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // パタンを作るオブジェクト
  auto patgen_option = ConfigParam(option).get_param("patgen");
  auto patgen = PatGen::new_obj(fault_info, patgen_option);

  // 等価故障/支配故障の候補を管理するオブジェクト
  auto candmgr_option = ConfigParam(option).get_param("candmgr");
  auto candmgr = EqDomMgr::new_obj(fault_list, candmgr_option);
  auto naive_mgr = EqDomMgr::new_dichotomy_mgr(fault_list, candmgr_option);
  auto max_size = network.max_fault_id();

  Timer fsim_timer;
  SizeType tv_count = 0;
  SizeType no_change = 0;
  while ( no_change < NO_CHANGE_LIMIT ) {
    std::vector<TestVector> tv_list(BATCH_SIZE);
    patgen->gen(BATCH_SIZE, tv_list);
    fsim_timer.start();
    auto res = fsim.run_multi(tv_list, true);
    fsim_timer.stop();
    tv_count += BATCH_SIZE;
    patgen->update(res);

    auto ntv = res.tv_num();
    std::vector<PackedVal> dpat_array(max_size, PV_ALL0);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      PackedVal bit = 1ULL << i;
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	dpat_array[fid] |= bit;
      }
    }
    auto change = candmgr->update(dpat_array);
    auto change2 = naive_mgr->update(dpat_array);
    if ( change != change2 ) {
      std::cout << "change = " << change
		<< ", change2 = " << change2 << std::endl;
      abort();
    }
    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
    auto cand1 = candmgr->end(true);
    auto cand2 = naive_mgr->end(true);
    if ( *cand1 != *cand2 ) {
      std::cout << "cand1" << std::endl;
      cand1->print(std::cout);
      std::cout << std::endl;
      std::cout << "naive_cand" << std::endl;
      cand2->print(std::cout);
      std::cout << std::endl;
      cand1->check(*cand2);
      abort();
    }
  }

  // 結果の EqDomCand を作る．
  auto cand = candmgr->end(true);
  auto naive_cand = naive_mgr->end(true);
  timer.stop();

  if ( *cand != *naive_cand ) {
    std::cout << "cand" << std::endl;
    cand->print(std::cout);
    std::cout << std::endl;
    std::cout << "naive_cand" << std::endl;
    naive_cand->print(std::cout);
    std::cout << std::endl;
    cand->check(*naive_cand);
  }

  if ( verbose ) {
    std::cout << "# of faults:            "
	      << std::setw(8) << std::right << fault_list.size() << std::endl
	      << "# of Groups:            "
	      << std::setw(8) << std::right << cand->group_num() << std::endl
	      << "Total # of patterns:    "
	      << std::setw(8) << std::right << tv_count << std::endl
	      << "No Change Limit:        "
	      << std::setw(8) << std::right << NO_CHANGE_LIMIT << std::endl
	      << "Filtering time:           " << time_str(timer) << std::endl
	      << " (Fsim time):             " << time_str(fsim_timer) << std::endl;
  }

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::filter_test(argc, argv);
}
