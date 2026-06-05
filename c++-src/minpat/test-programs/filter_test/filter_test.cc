
/// @file filter_test.cc
/// @brief DiFilter/NaiveFilter のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Filter.h"
#include "EqDomCand.h"
#include "FaultAnalyze.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"
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

  auto fault_list = network.rep_fault_list();

  FaultInfo fault_info;
  {
    auto analyze_option = ConfigParam(option).get_param("analyze");
    fault_info = FaultAnalyze::run(fault_list, analyze_option);
    auto rep_fault_list = fault_info.rep_fault_list();
    std::cout << "# of initial faults: " << rep_fault_list.size() << std::endl;
  }

  // Naive method
  std::cout << std::endl;
  std::cout << "Naive" << std::endl;
  auto naive_option = option;
  {
    auto filter_option = JsonValue::object();
    if ( no_change_limit > 0 ) {
      filter_option.add("no_change_limit", no_change_limit);
    }
    if ( batch_size > 0 ) {
      filter_option.add("batch_size", batch_size);
    }
    auto cand_option = JsonValue::object();
    cand_option.add("method", std::string{"naive"});
    filter_option.add("candmgr", cand_option);
    filter_option.add("*", global_option);
    naive_option.add("filter", filter_option);
  }

  auto cand1 = Filter::run(fault_info, ConfigParam(naive_option).get_param("filter"));

  // Dichotomy method
  std::cout << std::endl;
  std::cout << "Dichotomy" << std::endl;
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
    cand_option.add("method", std::string{"dichotomy2"});
    filter_option.add("candmgr", cand_option);
    dicho_option.add("filter", filter_option);
  }

  auto cand2 = Filter::run(fault_info, ConfigParam(dicho_option).get_param("filter"));

  if ( cand1 != cand2 ) {
    std::cout << "cand1" << std::endl;
    cand1.print(std::cout);
    std::cout << "cand2" << std::endl;
    cand2.print(std::cout);
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
