
/// @file analyze_test.cc
/// @brief FaultAnalyze を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "dtpg/FaultAnalyze.h"
#include "dtpg/NaiveDtpgEngine.h"
#include "dtpg/NaiveDualEngine.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "ym/SatInitParam.h"
#include "ym/Timer.h"
#include <libgen.h>


BEGIN_NAMESPACE_DRUID

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " ?--ffr-reduction? --blif|--iscas89 <file>"
	    << std::endl;
}

int
analyze_test(
  int argc,
  char** argv
)
{
  std::string sat_type;
  std::ostream* sat_outp = nullptr;

  std::string format = "blif";

  bool sa_mode = false;
  bool td_mode = false;
  bool ffr_reduction = false;
  bool mffc_reduction = false;
  bool multi_thread = false;
  bool global_reduction = false;
  bool dump = false;
  bool verify = false;
  bool verbose = false;
  bool debug = false;
  bool dom_dump = false;
  std::string sat_log;
  bool show_untestable_faults = false;
  std::string just_type;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      auto arg = std::string(argv[pos]);
      if ( arg == "--sat_type" ) {
	++ pos;
	if ( pos < argc ) {
	  sat_type = argv[pos];
	}
	else {
	  std::cerr << "--sat_type requires <string> argument"
		    << std::endl;
	  return -1;
	}
      }
      else if ( arg == "--blif" ) {
	format = "blif";
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	format = "iscas89";
      }
      else if ( arg == "--stuck-at" ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( arg == "--transition-delay" ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	td_mode = true;
      }
      else if ( arg == "--just_naive" ) {
	if ( !just_type.empty() ) {
	  std::cerr << "--just_naive and " << just_type << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	just_type = "naive";
      }
      else if ( strcmp(argv[pos], "--just1") == 0 ) {
	if ( !just_type.empty() ) {
	  std::cerr << "--just1 and " << just_type << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	just_type = "just1";
      }
      else if ( strcmp(argv[pos], "--just2") == 0 ) {
	if ( !just_type.empty() ) {
	  std::cerr << "--just2 and " << just_type << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	just_type = "just2";
      }
      else if ( arg == "--multi-thread" ) {
	multi_thread = true;
      }
      else if ( arg == "--ffr-reduction" ) {
	ffr_reduction = true;
      }
      else if ( arg == "--mffc-reduction" ) {
	mffc_reduction = true;
      }
      else if ( arg == "--global-reduction" ) {
	global_reduction = true;
      }
      else if ( arg == "--dump" ) {
	dump = true;
      }
      else if ( arg == "--verify" ) {
	verify = true;
      }
      else if ( arg == "--verbose" ) {
	verbose = true;
      }
      else if ( arg == "--debug" ) {
	debug = true;
      }
      else if ( arg == "--dom-dump" ) {
	dom_dump = true;
      }
      else if ( arg == "--sat_log" ) {
	++ pos;
	if ( pos < argc ) {
	  sat_log = argv[pos];
	}
	else {
	  std::cerr << "--sat_log requires <string> argument"
		    << std::endl;
	  return -1;
	}
      }
      else {
	std::cerr << argv[pos] << ": illegal option"
		  << std::endl;
	usage();
	return -1;
      }
    }
    else {
      break;
    }
  }

  if ( pos != argc - 1 ) {
    usage();
    return -1;
  }

  if ( !sa_mode && !td_mode ) {
    // sa_mode をデフォルトにする．
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  auto filename = std::string{argv[pos]};
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  if ( dump ) {
    network.print(std::cout);
  }

  auto global_option = JsonValue::object();
  if ( !just_type.empty() ) {
    global_option.add("justifier", just_type);
  }
  auto sat_option = JsonValue::object();
  if ( !sat_type.empty() ) {
    sat_option.add("type", sat_type);
  }
  if ( !sat_log.empty() ) {
    auto log_option = JsonValue::object();
    if ( sat_log == "stdout" ) {
      log_option.add("stdout", true);
    }
    else if ( sat_log == "stderr" ) {
      log_option.add("stderr", true);
    }
    else {
      log_option.add("file", sat_log);
    }
    sat_option.add("log", log_option);
  }
  global_option.add("sat_param", sat_option);
  global_option.add("multi_thread", multi_thread);
  global_option.add("ffr_reduction", ffr_reduction);
  global_option.add("mffc_reduction", mffc_reduction);
  global_option.add("global_reduction", global_reduction);
  global_option.add("debug", debug);
  global_option.add("verbose", verbose);
  auto option = JsonValue::object();
  option.add("*", global_option);

  Timer timer;
  timer.start();

  auto fault_list = network.rep_fault_list();
  auto analyze_option = ConfigParam(option).get_param("analyze");
  auto fault_info = FaultAnalyze::run(fault_list, analyze_option);

  timer.stop();
  auto time = timer.get_time();

  SizeType nd = 0;
  for ( auto fault: fault_list ) {
    if ( fault_info.is_rep(fault) ) {
      ++ nd;
    }
  }
  std::cout << "=========================================" << std::endl
	    << "# of initial faults:     " << fault_list.size() << std::endl
	    << "# of dominant faults:    " << nd << std::endl
	    << "Total CPU time:          " << time << "ms" << std::endl;

  if ( verify ) {
    int error = 0;
    std::vector<TpgFault> det_list;
    std::vector<bool> ref_rep_mark(network.max_fault_id(), false);
    det_list.reserve(fault_list.size());
    for ( auto fault: fault_list ) {
      NaiveDtpgEngine engine(fault);
      auto res = engine.solver().solve();
      auto ref_status = FaultStatus::Undetected;
      switch ( res ) {
      case SatBool3::True: ref_status = FaultStatus::Detected; break;
      case SatBool3::False: ref_status = FaultStatus::Untestable; break;
      default: break;
      }
      if ( fault_info.status(fault) != ref_status ) {
	++ error;
	std::cout << fault.str()
		  << ": status mismatch"
		  << "std::endl";
      }
      if ( res == SatBool3::True ) {
	det_list.push_back(fault);
	ref_rep_mark[fault.id()] = true;
      }
    }
    auto nd = det_list.size();
    for ( auto fault1: det_list ) {
      if ( fault_info.is_dominated(fault1) ) {
	auto fault2 = fault_info.dominator(fault1);
	NaiveDualEngine engine(fault1, fault2);
	auto res = engine.solve(false, true);
	if ( res != SatBool3::False ) {
	  ++ error;
	  std::cout << "fault1 = " << fault1.str()
		    << " @FFR#" << network.ffr(fault1).id() << std::endl
		    << "dominator = " << fault2.str()
		    << " @FFR#" << network.ffr(fault2).id() << std::endl
		    << "dominance check failed." << std::endl
		    << "expected to be dominated" << std::endl;
	}
      }
      else {
	for ( auto fault2: det_list ) {
	  if ( fault2 == fault1 ) {
	    continue;
	  }
	  NaiveDualEngine engine(fault1, fault2);
	  auto res = engine.solve(false, true);
	  if ( res != SatBool3::True ) {
	  ++ error;
	  std::cout << "fault1 = " << fault1.str()
		    << " @FFR#" << network.ffr(fault1).id() << std::endl
		    << "dominator = " << fault2.str()
		    << " @FFR#" << network.ffr(fault2).id() << std::endl
		    << "dominance check failed." << std::endl
		    << "expected not to be dominated" << std::endl;
	  }
	}
      }
    }
    if ( error ) {
      return error;
    }
  }

  if ( dom_dump ) {
    for ( auto fault: fault_list ) {
      if ( fault_info.is_rep(fault) ) {
	std::cout << fault.str() << std::endl;
      }
    }
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
  return DRUID_NAMESPACE::analyze_test(argc, argv);
}
