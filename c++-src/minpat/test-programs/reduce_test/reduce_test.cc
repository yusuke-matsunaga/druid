
/// @file reduce_test.cc
/// @brief dichotomy のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "MpReduce.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "types/TpgFaultList.h"
#include "ym/JsonValue.h"
#include "ym/Timer.h"
#include <unistd.h> // getopt
#include <libgen.h> // basename


BEGIN_NAMESPACE_DRUID

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

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " [--blif|--iscas89] <file>"
	    << std::endl;
}

int
reduce_test(
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

  auto option = JsonValue::object();
  {
    auto global_option = JsonValue::object();
    global_option.add("multi_thread", multi_thread);
    global_option.add("verbose", verbose);
    global_option.add("debug", debug);
    option.add("*", global_option);
  }
  option.add("ffr_reduction", ffr_reduction);
  option.add("mffc_reduction", mffc_reduction);
  option.add("global_reduction", false);
  if ( no_change_limit > 0 ) {
    option.add("no_change_limit", no_change_limit);
  }
  if ( batch_size > 0 ) {
    option.add("batch_size", batch_size);
  }
  if ( sat_time_limit > 0 ) {
    option.add("time_limit", sat_time_limit);
  }

  Timer timer;
  timer.start();

  auto fault_list = network.rep_fault_list();
  auto red_fault_list = MpReduce::run(fault_list, ConfigParam(option));

  timer.stop();

  if ( verbose ) {
    std::cout << "# of initial faults: " << fault_list.size() << std::endl
	      << "# of reduced faults: " << red_fault_list.size() << std::endl
	      << "Total CPU time:      " << time_str(timer) << std::endl;
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
  return DRUID_NAMESPACE::reduce_test(argc, argv);
}
