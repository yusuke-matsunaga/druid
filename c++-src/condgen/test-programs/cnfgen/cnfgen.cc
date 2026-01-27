
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
#include "CondGen.h"
#include "condgen/CondGenMgr.h"
#include "condgen/CondGenStats.h"
#include "dtpg/BdEngine.h"
#include "ym/CnfSize.h"
#include "ym/Timer.h"
#include <random>
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
  bool multi = false;
  bool verbose = false;
  std::string just_type;
  int limit = 100;
  bool cover = false;
  bool bdd = false;
  bool factor = false;
  bool aig = false;
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
      else if ( strcmp(argv[pos], "--cover") == 0 ) {
	cover = true;
      }
      else if ( strcmp(argv[pos], "--bdd") == 0 ) {
	bdd = true;
      }
      else if ( strcmp(argv[pos], "--factor") == 0 ) {
	factor = true;
      }
      else if ( strcmp(argv[pos], "--aig") == 0 ) {
	aig = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
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

  std::unordered_map<std::string, JsonValue> option_dict;
  if ( !just_type.empty() ) {
    option_dict.emplace("justifier", just_type);
  }
  if ( !sat_type.empty() ) {
    auto sat_obj = JsonValue{sat_type};
    option_dict.emplace("sat_param", sat_obj);
  }
  JsonValue option{option_dict};

  std::unordered_map<std::string, JsonValue> cg_option_dict;
  if ( !sat_type.empty() ) {
    auto sat_obj = JsonValue{sat_type};
    cg_option_dict.emplace("sat_param", sat_obj);
  }
  cg_option_dict.emplace("debug", JsonValue{debug_level});

  cg_option_dict.emplace("loop_limit", limit);

  JsonValue naive_cg_option{cg_option_dict};

  std::unordered_map<std::string, JsonValue> cnf_option_dict;
  if ( cover ) {
    cnf_option_dict.emplace("method", "cover");
  }
  else if ( bdd ) {
    cnf_option_dict.emplace("method", "bdd");
  }
  else if ( factor ) {
    cnf_option_dict.emplace("method", "factor");
  }
  else if ( aig ) {
    cnf_option_dict.emplace("method", "aig");
  }
  JsonValue cnf_option{cnf_option_dict};

  cg_option_dict.emplace("cnfgen", cnf_option);

  JsonValue cg_option{cg_option_dict};

  Timer total_timer;
  total_timer.start();

  for ( auto ffr: network.ffr_list() ) {
    BdEngine engine(network, ffr.root(), option);
    engine.add_prev_node(ffr.root());
    auto cond = CondGen::root_cond(network, ffr, 1000, cg_option);
    if ( cond.type() == DetCond::Undetected ) {
      continue;
    }
    CondGenStats stats;
    auto lits_list = CondGenMgr::make_cnf(engine, {cond}, cnf_option, stats);
    auto& lits = lits_list.front();
    auto assumptions = lits;
    auto pvar = engine.prop_var();
    auto assumptions1 = assumptions;
    assumptions1.push_back(~pvar);
    auto res = engine.solver().solve(assumptions1);
    std::cout << res
	      << std::endl;
    auto size = engine.solver().cnf_size();
    std::cout << size
	      << std::endl;
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
