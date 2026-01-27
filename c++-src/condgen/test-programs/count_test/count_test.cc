
/// @file count_test.cc
/// @brief CnfGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/FaultType.h"
#include "fsim/Fsim.h"
#include "condgen/CondGenMgr.h"
#include "condgen/CondGenStats.h"
#include "dtpg/StructEngine.h"
#include "ym/CnfSize.h"
#include "ym/Timer.h"
#include <libgen.h>


BEGIN_NAMESPACE_DRUID

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " --blif|--iscas89 <file>"
	    << std::endl;
}

void
print_cnf_size(
  const CnfSize& size
)
{
  std::cout << " #C:" << std::setw(8) << size.clause_num
	    << " #L:" << std::setw(8) << size.literal_num;
}

void
print_stats(
  const std::string& method,
  const CnfSize& size,
  double time
)
{
  std::cout << std::setw(8) << std::left << method
	    << std::right
	    << "|";
  if ( method == "condgen" ) {
    std::cout << "                        ";
  }
  else {
    print_cnf_size(size);
  }
  std::cout << " | "
	    << std::setw(10) << std::fixed << std::setprecision(2)
	    << time
	    << std::endl;
}

int
count_test(
  int argc,
  char** argv
)
{
  std::string sat_type;
  std::string sat_option;
  std::ostream* sat_outp = nullptr;

  std::string format = "blif";

  bool sa_mode = false;
  bool td_mode = false;
  bool verbose = false;
  std::string just_type;
  int limit = 100;
  bool multi = false;
  int debug_level = 0;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--sat_type") == 0 ) {
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
      else if ( strcmp(argv[pos], "--sat_option") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  sat_option = argv[pos];
	}
	else {
	  std::cerr << "--sat_option requires <string> argument"
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
      else if ( strcmp(argv[pos], "--bt1") == 0 ) {
	if ( just_type != "" ) {
	  std::cerr << "--bt0, --bt1, and --bt2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	just_type = "just1";
      }
      else if ( strcmp(argv[pos], "--bt2") == 0 ) {
	if ( just_type != "" ) {
	  std::cerr << "--bt0, --bt1, and --bt2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	just_type = "just2";
      }
      else if ( strcmp(argv[pos], "--limit") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  limit = atoi(argv[pos]);
	}
	else {
	  std::cerr << "--limit requires <int> argument"
		    << std::endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
      }
      else if ( strcmp(argv[pos], "--debug") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  debug_level = atoi(argv[pos]);
	}
	else {
	  std::cerr << "--debug requires <int> argument"
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

  std::string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  if ( verbose ) {
    std::cout << "Total " << network.rep_fault_list().size() << " faults"
	      << std::endl;
  }

  auto dtpg_option = JsonValue::object();
  if ( just_type != "" ) {
    dtpg_option.add("justifier", JsonValue{just_type});
  }
  if ( !sat_type.empty() ) {
    auto sat_obj = JsonValue{sat_type};
    dtpg_option.add("sat_param", sat_obj);
  }

  auto cg_option = JsonValue::object();
  if ( !sat_type.empty() ) {
    auto sat_obj = JsonValue{sat_type};
    cg_option.add("sat_param", sat_obj);
  }
  cg_option.add("debug", JsonValue{debug_level});
  cg_option.add("loop_limit", JsonValue{limit});
  if ( multi ) {
    cg_option.add("multi_thread", JsonValue{true});
  }

  Timer total_timer;
  total_timer.start();

  {
    Timer timer;
    timer.start();
    StructEngine engine(network, dtpg_option);
    CondGenStats stats;
    CondGenMgr::make_cnf_naive(engine, cg_option, stats);
    timer.stop();
    auto size = engine.solver().cnf_size();
    print_stats("raw", size, timer.get_time());
  }

  Timer condgen_timer;
  condgen_timer.start();

  auto cond_list = CondGenMgr::make_cond(network, cg_option);

  condgen_timer.stop();
  print_stats("condgen", CnfSize::zero(), condgen_timer.get_time());

  for ( std::string method: {"sop", "factor", "aig", "aig2"} ) {
    Timer timer;
    timer.start();
    auto option = JsonValue::object();
    if ( method == "aig2" ) {
      method = "aig";
      option.add("rewrite", JsonValue(true));
    }
    option.add("method", JsonValue(method));
    if ( multi ) {
      option.add("multi_thread", JsonValue(true));
    }
    StructEngine engine(network, cg_option);
    CondGenStats stats;
    CondGenMgr::make_cnf(engine, cond_list, option, stats);
    timer.stop();
    auto size = engine.solver().cnf_size();
    print_stats(method, size, timer.get_time());
    {
      std::cout << "                       base = " << stats.base_size()
		<< std::endl
		<< "                       bd   = " << stats.bd_size()
		<< std::endl
		<< "                       cond = " << stats.cond_size()
		<< std::endl;
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
  return DRUID_NAMESPACE::count_test(argc, argv);
}
