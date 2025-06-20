
/// @file count_test.cc
/// @brief CnfGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "Fsim.h"
#include "CondGenMgr.h"
#include "CondGenStats.h"
#include "StructEngine.h"
#include "ym/CnfSize.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
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
  const string& method,
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
	    << time;
  std::cout << endl;
}

int
count_test(
  int argc,
  char** argv
)
{
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;

  string format = "blif";

  bool sa_mode = false;
  bool td_mode = false;
  bool verbose = false;
  string just_type;
  int limit = 100;
  bool cover = false;
  bool bdd = false;
  bool factor = false;
  bool aig = false;
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
	  cerr << "--sat_type requires <string> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--sat_option") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  sat_option = argv[pos];
	}
	else {
	  cerr << "--sat_option requires <string> argument" << endl;
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
	  cerr << "--stuck-at and --transition-delay are mutually exclusive" << endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( strcmp(argv[pos], "--transition-delay") == 0 ) {
	if ( td_mode ) {
	  cerr << "--stuck-at and --transition-delay are mutually exclusive" << endl;
	  return -1;
	}
	td_mode = true;
      }
      else if ( strcmp(argv[pos], "--bt1") == 0 ) {
	if ( just_type != "" ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	just_type = "just1";
      }
      else if ( strcmp(argv[pos], "--bt2") == 0 ) {
	if ( just_type != "" ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
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
	  cerr << "--limit requires <int> argument" << endl;
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
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
      }
      else if ( strcmp(argv[pos], "--debug") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  debug_level = atoi(argv[pos]);
	}
	else {
	  cerr << "--debug requires <int> argument" << endl;
	  return -1;
	}
      }
      else {
	cerr << argv[pos] << ": illegal option" << endl;
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

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  if ( verbose ) {
    cout << "Total " << network.rep_fault_list().size() << " faults" << endl;
  }

  unordered_map<string, JsonValue> option_dict;
  if ( just_type != "" ) {
    option_dict.emplace("justifier", just_type);
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    option_dict.emplace("sat_param", sat_obj);
  }
  JsonValue option{option_dict};

  unordered_map<string, JsonValue> cg_option_dict;
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    cg_option_dict.emplace("sat_param", sat_obj);
  }
  cg_option_dict.emplace("debug", JsonValue{debug_level});

  cg_option_dict.emplace("loop_limit", limit);

  JsonValue naive_cg_option{cg_option_dict};

  if ( multi ) {
    cg_option_dict.emplace("multi_thread", JsonValue{true});
  }

  JsonValue cg_option{cg_option_dict};

  Timer total_timer;
  total_timer.start();

  Timer condgen_timer;
  condgen_timer.start();

  auto cond_list = CondGenMgr::make_cond(network, cg_option);

  condgen_timer.stop();
  print_stats("condgen", CnfSize::zero(), condgen_timer.get_time());

  {
    Timer timer;
    timer.start();
    StructEngine engine(network, cg_option);
    CondGenStats stats;
    CondGenMgr::make_cnf_naive(engine, cg_option, stats);
    timer.stop();
    auto size = engine.solver().cnf_size();
    print_stats("raw", size, timer.get_time());
  }

  for ( auto method: {"sop", "factor", "aig", "aig2"} ) {
    Timer timer;
    timer.start();
    std::unordered_map<string, JsonValue> json_dict;
    if ( method == string{"aig2"} ) {
      method = "aig";
      json_dict.emplace("rewrite", JsonValue(true));
    }
    json_dict.emplace("method", JsonValue(method));
    if ( multi ) {
      json_dict.emplace("multi_thread", JsonValue(true));
    }
    auto option = JsonValue(json_dict);
    StructEngine engine(network, cg_option);
    CondGenStats stats;
    CondGenMgr::make_cnf(engine, cond_list, option, stats);
    timer.stop();
    auto size = engine.solver().cnf_size();
    print_stats(method, size, timer.get_time());
    {
      std::cout << "                       base = " << stats.base_size() << endl
		<< "                       bd   = " << stats.bd_size() << endl
		<< "                       cond = " << stats.cond_size() << endl;
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
