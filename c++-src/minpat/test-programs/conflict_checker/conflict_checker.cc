
/// @file conflict_checker.cc
/// @brief ConflictChecker を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "FaultInfo.h"
#include "FaultInfoMgr.h"
#include "Reducer.h"
#include "NaiveDomChecker.h"
#include "NaiveDomChecker2.h"
#include "ConflictChecker.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

int
conflict_checker(
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
  bool naive = false;
  bool naive2 = false;
  bool se = false;
  bool verbose = false;
  bool do_trivial_check = true;
  bool conflict_check = false;
  bool localimp = false;
  bool globalimp = false;
  string just_type;
  int loop = 1;

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
      else if ( strcmp(argv[pos], "--localimp") == 0 ) {
	localimp = true;
      }
      else if ( strcmp(argv[pos], "--globalimp") == 0 ) {
	globalimp = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
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

  unordered_map<string, JsonValue> option_dict;
  option_dict.emplace("just_type", just_type);
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    option_dict.emplace("sat_param", sat_obj);
  }
  option_dict.emplace("debug", JsonValue{true});
  JsonValue option{option_dict};

  auto fault_list = network.rep_fault_list();

  FaultInfoMgr finfo_mgr{network, fault_list};

  finfo_mgr.generate(option);

  vector<const TpgFault*> det_fault_list;
  det_fault_list.reserve(fault_list.size());
  for ( auto fault: fault_list ) {
    auto& finfo = finfo_mgr.fault_info(fault);
    if ( finfo.status() == FaultStatus::Detected ) {
      det_fault_list.push_back(fault);
    }
  }

  unordered_map<string, JsonValue> fr_option_dict;
  fr_option_dict.emplace("loop_limit", JsonValue{loop});
  fr_option_dict.emplace("do_trivial_check", JsonValue{do_trivial_check});
  JsonValue fr_option{fr_option_dict};

  Timer timer;
  timer.start();

  auto red_fault_list = Reducer::reduce(finfo_mgr, fr_option);

  timer.stop();

  cout << "Detected Faults: " << det_fault_list.size() << endl
       << "Reduced Faults:  " << red_fault_list.size() << endl
       << "CPU time:        " << timer.get_time() << endl
       << "================================" << endl;

  unordered_map<string, JsonValue> cc_option_dict;
  cc_option_dict.emplace("debug", JsonValue{true});
  cc_option_dict.emplace("localimp", JsonValue{localimp});
  cc_option_dict.emplace("globalimp", JsonValue{globalimp});
  JsonValue cc_option{cc_option_dict};

  ConflictChecker checker{finfo_mgr};

  checker.run(cc_option);

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::conflict_checker(argc, argv);
}
