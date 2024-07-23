
/// @file fault_reducer.cc
/// @brief FaultReducer を使ったサンプルプログラム
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
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

int
fault_reducer(
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
      else if ( strcmp(argv[pos], "--loop") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  loop = atoi(argv[pos]);
	}
	else {
	  cerr << "--loop requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--naive") == 0 ) {
	naive = true;
      }
      else if ( strcmp(argv[pos], "--naive2") == 0 ) {
	naive2 = true;
      }
      else if ( strcmp(argv[pos], "--no-trivial-check") == 0 ) {
	do_trivial_check = false;
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

  if ( naive ) {
    SizeType n = det_fault_list.size();
    vector<bool> deleted(n, false);
    JsonValue option;
    for ( SizeType i1 = 0; i1 < n; ++ i1 ) {
      if ( deleted[i1] ) {
	continue;
      }
      auto f1 = det_fault_list[i1];
      for ( SizeType i2 = 0; i2 < n; ++ i2 ) {
	if ( i2 == i1 ) {
	  continue;
	}
	if ( deleted[i2] ) {
	  continue;
	}
	auto f2 = det_fault_list[i2];
	NaiveDomChecker checker{network, f1, f2, option};
	if ( checker.check() ) {
	  deleted[i2] = true;
	}
      }
    }
    SizeType n2 = 0;
    for ( SizeType i = 0; i < n; ++ i ) {
      if ( !deleted[i] ) {
	++ n2;
      }
    }

    cout << "Detected Faults: " << n << endl
	 << "Reduced Faults:  " << n2 << endl;
  }
  else if ( naive2 ) {
    SizeType n = det_fault_list.size();
    vector<bool> deleted(n, false);
    JsonValue option;
    for ( SizeType i1 = 0; i1 < n; ++ i1 ) {
      if ( deleted[i1] ) {
	continue;
      }
      auto f1 = det_fault_list[i1];
      for ( SizeType i2 = 0; i2 < n; ++ i2 ) {
	if ( i2 == i1 ) {
	  continue;
	}
	if ( deleted[i2] ) {
	  continue;
	}
	auto f2 = det_fault_list[i2];
	NaiveDomChecker2 checker{network, f1, f2, option};
	if ( checker.check() ) {
	  deleted[i2] = true;
	}
      }
    }
    SizeType n2 = 0;
    for ( SizeType i = 0; i < n; ++ i ) {
      if ( !deleted[i] ) {
	++ n2;
      }
    }

    cout << "Detected Faults: " << n << endl
	 << "Reduced Faults:  " << n2 << endl;
  }
  else {
    unordered_map<string, JsonValue> fr_option_dict;
    fr_option_dict.emplace("debug", JsonValue{true});
    fr_option_dict.emplace("loop_limit", JsonValue{loop});
    fr_option_dict.emplace("do_trivial_check", JsonValue{do_trivial_check});
    JsonValue fr_option{fr_option_dict};

    Timer timer;
    timer.start();

    auto fault_list = Reducer::reduce(finfo_mgr, fr_option);

    timer.stop();

    SizeType nr = fault_list.size();
    cout << "Detected Faults: " << det_fault_list.size() << endl
	 << "Reduced Faults:  " << nr << endl
	 << "CPU time:        " << timer.get_time() << endl;
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
  return DRUID_NAMESPACE::fault_reducer(argc, argv);
}
