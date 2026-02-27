
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "dtpg/DtpgMgr.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "ym/SatInitParam.h"
#include "ym/Timer.h"
#include <libgen.h>


BEGIN_NAMESPACE_DRUID

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " ?--mffc? --blif|--iscas89 <file>"
	    << std::endl;
}

// @brief 統計情報を出力する．
void
print_stats(
  const std::string& mode,
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  const DtpgResults& results,
  double time
)
{
  SizeType fault_num = 0;
  SizeType detect_num = 0;
  SizeType untest_num = 0;
  for ( auto fault: fault_list ) {
    auto fs = results.status(fault);
    switch ( fs ) {
    case FaultStatus::Detected: ++ detect_num; break;
    case FaultStatus::Untestable: ++ untest_num; break;
    default: break;
    }
    ++ fault_num;
  }
  std::cout << " DTPG mode              = " << mode << std::endl
	    << "# of inputs             = " << network.input_num() << std::endl
	    << "# of outputs            = " << network.output_num() << std::endl
	    << "# of DFFs               = " << network.dff_num() << std::endl
	    << "# of logic gates        = " << network.node_num() - network.ppi_num()
	    << std::endl
	    << "# of MFFCs              = " << network.mffc_num() << std::endl
	    << "# of FFRs               = " << network.ffr_num() << std::endl
	    << "# of total faults       = " << fault_num << std::endl
	    << "# of detected faults    = " << detect_num << std::endl
	    << "# of untestable faults  = " << untest_num << std::endl
	    << "Total CPU time(s)       = " << (time / 1000.0) << std::endl;

  auto save = std::cout.flags();
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  if ( results.detect_count() > 0 ) {
    std::cout << std::endl
	      << "*** SAT instances (" << results.detect_count() << ") ***" << std::endl
	      << "Total CPU time  (s)            = "
	      << std::setw(10) << (results.detect_time() / 1000.0) << std::endl
	      << "Average CPU time (ms)          = "
	      << std::setw(10) << (results.detect_time() / results.detect_count()) << std::endl;
  }
  if ( results.untest_count() > 0 ) {
    std::cout << std::endl
	      << "*** UNSAT instances (" << results.untest_count() << ") ***" << std::endl
	      << "Total CPU time  (s)            = "
	      << std::setw(10) << (results.untest_time() / 1000.0) << std::endl
	      << "Average CPU time (ms)          = "
	      << std::setw(10) << results.untest_time() / results.untest_count() << std::endl;
  }
  if ( results.abort_count() > 0 ) {
    std::cout << std::endl
	      << "*** ABORT instances ***" << std::endl
	      << "  " << std::setw(10) << results.abort_count()
	      << "  " << results.abort_time()
	      << "  " << std::setw(8) << results.abort_time() / results.abort_count() << std::endl;
  }

  std::cout << std::endl
	    << "SAT statistics" << std::endl
	    << std::endl
	    << "CNF generation" << std::endl
	    << "  " << std::setw(10) << results.cnfgen_count()
	    << "  " << (results.cnfgen_time() / 1000.0)
	    << "  " << std::setw(8) << results.cnfgen_time() / results.cnfgen_count()
	    << std::endl
	    << std::endl
	    << "# of restarts (Ave./Max)       = "
	    << std::setw(10) << (double) results.sat_stats().mRestart / results.total_count()
	    << " / " << std::setw(8) << results.sat_stats_max().mRestart << std::endl

	    << "# of conflicts (Ave./Max)      = "
	    << std::setw(10) << (double) results.sat_stats().mConflictNum / results.total_count()
	    << " / " << std::setw(8) << results.sat_stats_max().mConflictNum << std::endl

	    << "# of decisions (Ave./Max)      = "
	    << std::setw(10) << (double) results.sat_stats().mDecisionNum / results.total_count()
	    << " / " << std::setw(8) << results.sat_stats_max().mDecisionNum << std::endl

	    << "# of implications (Ave./Max)   = "
	    << std::setw(10) << (double) results.sat_stats().mPropagationNum / results.total_count()
	    << " / " << std::setw(8) << results.sat_stats_max().mPropagationNum << std::endl;

  std::cout << std::endl
	    << "*** backtrace time ***" << std::endl
	    << "  " << (results.backtrace_time() / 1000.0)
	    << "  " << std::setw(8) << results.backtrace_time() / results.detect_count() << std::endl;
  std::cout.flags(save);
}

int
dtpg_test(
  int argc,
  char** argv
)
{
  std::string sat_type;
  std::ostream* sat_outp = nullptr;

  std::string format = "blif";

  bool sa_mode = false;
  bool td_mode = false;
  std::string mode{};
  bool drop = false;
  bool fix = false;
  bool gtc = false;
  bool multi = false;
  bool dump = false;
  bool verbose = false;
  std::string sat_log;
  bool show_untestable_faults = false;
  std::string just_type;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--ffr") == 0 ) {
	if ( !mode.empty() ) {
	  std::cerr << "--ffr and --" << mode << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	mode = "ffr";
      }
      else if ( strcmp(argv[pos], "--mffc") == 0 ) {
	if ( !mode.empty() ) {
	  std::cerr << "--mffc and --" << mode << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	mode = "mffc";
      }
      else if ( strcmp(argv[pos], "--node") == 0 ) {
	if ( !mode.empty() ) {
	  std::cerr << "--node and --" << mode << " are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	mode = "node";
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
      else if ( strcmp(argv[pos], "--drop") == 0 ) {
	drop = true;
      }
      else if ( strcmp(argv[pos], "--fix") == 0 ) {
	fix = true;
      }
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
      }
      else if ( strcmp(argv[pos], "--gtc") == 0 ) {
	gtc = true;
      }
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--show_untestable_faults") == 0 ) {
	show_untestable_faults = true;
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

  if ( mode.empty() ) {
    // ffr をデフォルトにする．
    mode = "ffr";
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

  auto option = JsonValue::object();
  option.add("group_mode",  mode);
  option.add("gtc", gtc);
  if ( !just_type.empty() ) {
    option.add("justifier", just_type);
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
  option.add("sat_param", sat_option);

  Timer timer;
  timer.start();

  auto fault_list = network.rep_fault_list();

  auto results = DtpgMgr::run(fault_list, option);

  timer.stop();
  auto time = timer.get_time();

  if ( verbose ) {
    print_stats(mode, network, fault_list, results, time);
  }

  if ( show_untestable_faults ) {
    std::cout << "Untestabel faults"
	      << std::endl;
    for ( auto fault: fault_list ) {
      if ( results.status(fault) == FaultStatus::Untestable ) {
	std::cout << fault
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
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}
