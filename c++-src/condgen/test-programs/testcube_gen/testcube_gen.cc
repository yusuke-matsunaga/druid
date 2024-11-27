
/// @file testcube_gen.cc
/// @brief TestCoverGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "DtpgMgr.h"
#include "DomCandMgr.h"
#include "Reducer.h"
#include "TestCoverGen.h"
#include "TestCond.h"
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
testcube_gen(
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
  int loop = 1;
  int limit = 1;
  bool do_finfo_mgr = false;
  bool do_reduction = true;
  bool do_ffr_reduction = false;
  bool do_global_reduction = false;
  bool expr_gen = false;
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
      else if ( strcmp(argv[pos], "--expr_gen") == 0 ) {
	expr_gen = true;
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

  unordered_map<string, JsonValue> tcg_option_dict;
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    tcg_option_dict.emplace("sat_param", sat_obj);
  }
  tcg_option_dict.emplace("debug", JsonValue{debug_level});
  JsonValue tcg_option{tcg_option_dict};

  auto src_fault_list = network.rep_fault_list();

  Timer total_timer;
  total_timer.start();

  Timer dtimer;
  Timer rtimer;
  Timer ctimer;

  vector<const TpgFault*> det_fault_list;
  vector<const TpgFault*> fault_list;
  vector<TestCond> cond_list;

  SizeType total_cube_num = 0;
  SizeType total_literal_num = 0;

  if ( do_finfo_mgr ) {
    dtimer.start();

    FaultInfoMgr finfo_mgr{network, src_fault_list};

    finfo_mgr.generate(option);

    det_fault_list.reserve(fault_list.size());
    for ( auto fault: src_fault_list ) {
      auto& finfo = finfo_mgr.fault_info(fault);
      if ( finfo.status() == FaultStatus::Detected ) {
	det_fault_list.push_back(fault);
      }
    }
    dtimer.stop();

    unordered_map<string, JsonValue> fr_option_dict;
    fr_option_dict.emplace("debug", JsonValue{debug_level});
    fr_option_dict.emplace("loop_limit", JsonValue{loop});
    JsonValue fr_option{fr_option_dict};

    rtimer.start();

    if ( do_reduction ) {
      fault_list = Reducer::reduce(finfo_mgr, fr_option);
    }
    else {
      fault_list = det_fault_list;
    }

    rtimer.stop();

    ctimer.start();

    auto& fault_list = finfo_mgr.active_fault_list();
    cond_list = TestCoverGen::run(network, fault_list, limit, tcg_option);

    ctimer.stop();
  }
  else {
    dtimer.start();

    DtpgMgr mgr{network, network.rep_fault_list()};

    vector<TestVector> tv_list;
    std::mt19937 rg;

    mgr.run(
      // detected callback
      [&](DtpgMgr& mgr, const TpgFault* f, TestVector tv) {
	det_fault_list.push_back(f);
	tv.fix_x_from_random(rg);
	tv_list.push_back(tv);
      }
    );

    dtimer.stop();

    rtimer.start();

    DomCandMgr domcand_mgr{network};

    if ( do_ffr_reduction || do_global_reduction ) {
      domcand_mgr.generate(det_fault_list, tv_list, loop);
    }

    fault_list = det_fault_list;

    if ( do_ffr_reduction ) {
      unordered_map<string, JsonValue> option_dict;
      option_dict.emplace("debug", JsonValue{debug_level});
      JsonValue option{option_dict};

      fault_list = ffr_reduction(network, fault_list,
				 domcand_mgr, option);
    }

    if ( do_global_reduction ) {
      unordered_map<string, JsonValue> option_dict;
      option_dict.emplace("debug", JsonValue{debug_level});
      JsonValue option{option_dict};

      fault_list = global_reduction(network, fault_list,
				    domcand_mgr, option);
    }
    rtimer.stop();

    ctimer.start();

    cond_list = TestCoverGen::run(network, fault_list, limit, tcg_option);
#if 0
    for ( auto& cover: cover_list ) {
      total_cube_num += cover.cube_num();
      total_literal_num += cover.literal_num();
    }
#endif

    ctimer.stop();
  }

  cout << "# DTPG TIME  :  "
       << (dtimer.get_time() / 1000.0) << endl;
  cout << "# REDUCE TIME:  "
       << (rtimer.get_time() / 1000.0) << endl;
  cout << "# CUBEGen TIME: "
       << (ctimer.get_time() / 1000.0) << endl;

  total_timer.stop();
  cerr << "=========================================" << endl
       << "Detected Faults:     " << det_fault_list.size() << endl
       << "Reduced Faults:      " << fault_list.size() << endl
       << "Total # of cubes:    " << total_cube_num << endl
       << "Total # of literal:  " << total_literal_num << endl
       << "Total CPU time:      "
       << (total_timer.get_time() / 1000.0) << endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::testcube_gen(argc, argv);
}
