
/// @file condgen.cc
/// @brief TestCoverGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "Fsim.h"
#include "CondGenMgr.h"
#include "ym/Timer.h"
#include <random>


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

int
condgen(
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
  bool multi = false;
  bool verbose = false;
  string just_type;
  int limit = 1;
  bool ffr_mode = false;
  bool do_finfo_mgr = false;
  bool do_reduction = true;
  bool do_ffr_reduction = false;
  bool do_global_reduction = false;
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
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
      }
      else if ( strcmp(argv[pos], "--fault-info-mgr") == 0 ) {
	do_finfo_mgr = true;
      }
      else if ( strcmp(argv[pos], "--no-reduction") == 0 ) {
	do_reduction = false;
      }
      else if ( strcmp(argv[pos], "--ffr_reduction") == 0 ) {
	do_ffr_reduction = true;
      }
      else if ( strcmp(argv[pos], "--global_reduction") == 0 ) {
	do_global_reduction = true;
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
      else if ( strcmp(argv[pos], "--ffr") == 0 ) {
	ffr_mode = true;
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
  JsonValue cg_option{cg_option_dict};

  auto src_fault_list = network.rep_fault_list();

  Timer total_timer;
  total_timer.start();

  if ( ffr_mode ) {
    CondGenMgr::root_cond(network, limit,
			  [&](const TpgFFR* ffr,
			      const TestCond& cond,
			      SizeType count,
			      double time){
			    cout << "FFR#" << ffr->id()
				 << ": " << time << endl
				 << setw(4) << count
				 << "| " << cond.expr().literal_num() << endl;
			  },
			  cg_option);
  }
  else {
    CondGenMgr::fault_cond(network, src_fault_list, limit,
			   [&](const TpgFault* fault,
			       const TestCond& cond,
			       SizeType count,
			       double time){
			     cout << fault->str()
				  << ": " << time << endl
				  << setw(4) << count
				  << "| " << cond.expr().literal_num() << endl;
			   },
			   cg_option);
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
  return DRUID_NAMESPACE::condgen(argc, argv);
}
