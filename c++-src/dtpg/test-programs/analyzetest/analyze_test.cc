
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
  bool multi_thread = false;
  bool dump = false;
  bool verify = false;
  bool verbose = false;
  bool debug = false;
  std::string sat_log;
  bool show_untestable_faults = false;
  std::string just_type;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--ffr-reduction") == 0 ) {
	ffr_reduction = true;
      }
      else if ( strcmp(argv[pos], "--sat_type") == 0 ) {
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
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	format = "blif";
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	format = "iscas89";
      }
      else if ( strcmp(argv[pos], "--stuck-at") == 0 ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( strcmp(argv[pos], "--transition-delay") == 0 ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	td_mode = true;
      }
      else if ( strcmp(argv[pos], "--just_naive") == 0 ) {
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
      else if ( strcmp(argv[pos], "--multi-thread") == 0 ) {
	multi_thread = true;
      }
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
      }
      else if ( strcmp(argv[pos], "--verify") == 0 ) {
	verify = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--debug") == 0 ) {
	debug = true;
      }
      else if ( strcmp(argv[pos], "--sat_log") == 0 ) {
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
  global_option.add("debug", debug);
  global_option.add("verbose", verbose);
  auto option = JsonValue::object();
  //option.add("ffr_reduction", ffr_reduction);
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
	    << "# of dominannt faults:   " << nd << std::endl
	    << "Total CPU time:          " << time << "ms" << std::endl;

  if ( verify ) {
    std::vector<TpgFault> det_list;
    std::vector<bool> ref_rep_mark(network.max_fault_id(), false);
    det_list.reserve(fault_list.size());
    for ( auto fault: fault_list ) {
      NaiveDtpgEngine engine(fault);
      auto res = engine.solver().solve();
      if ( res == SatBool3::True ) {
	det_list.push_back(fault);
	ref_rep_mark[fault.id()] = true;
      }
    }
    auto nd = det_list.size();
    for ( SizeType i1 = 0; i1 < nd - 1; ++ i1 ) {
      auto fault1 = det_list[i1];
      if ( !ref_rep_mark[fault1.id()] ) {
	continue;
      }
      if ( debug ) {
	std::cout << "fault1 = " << fault1.str()
		  << " @FFR#" << network.ffr(fault1).id() << std::endl;
      }
      for ( SizeType i2 = i1 + 1; i2 < nd; ++ i2 ) {
	auto fault2 = det_list[i2];
	if ( !ref_rep_mark[fault2.id()] ) {
	  continue;
	}
	if ( debug ) {
	  std::cout << "  fault2 = " << fault2.str()
		    << " @FFR#" << network.ffr(fault2).id() << std::endl;
	}
	NaiveDualEngine engine(fault1, fault2);
	auto res = engine.solve(true, true);
	if ( res != SatBool3::True ) {
	  continue;
	}
	{ // fault1 を検出して fault2 を検出しない条件
	  auto res = engine.solve(true, false);
	  if ( res == SatBool3::False ) {
	    if ( debug ) {
	      std::cout << fault2.str()
			<< " is dominated by "
			<< fault1.str()
			<< std::endl;
	    }
	    ref_rep_mark[fault2.id()] = false;
	    continue;
	  }
	}
	{ // fault2 を検出して fault1 を検出しない条件
	  auto res = engine.solve(false, true);
	  if ( res == SatBool3::False ) {
	    if ( debug ) {
	      std::cout << fault1.str()
			<< " is dominated by "
			<< fault2.str()
			<< std::endl;
	    }
	    ref_rep_mark[fault1.id()] = false;
	    break;
	  }
	}
      }
    }
    for ( auto fault: fault_list ) {
      auto rep_mark = fault_info.is_rep(fault);
      if ( rep_mark != ref_rep_mark[fault.id()] ) {
	std::cout << fault.str()
		  << ": rep_mark = "
		  << rep_mark
		  << ", ref_rep_mark = "
		  << ref_rep_mark[fault.id()]
		  << std::endl;
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
