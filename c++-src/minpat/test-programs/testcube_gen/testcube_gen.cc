
/// @file testcube_gen.cc
/// @brief TestCubeGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "DtpgMgr.h"
#include "TpgFault.h"
#include "TestCubeGen.h"
#include "FaultGroupGen.h"
#include "ym/SatInitParam.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

// @brief 統計情報を出力する．
void
print_stats(
  const DtpgMgr& mgr,
  const DtpgStats& stats,
  const vector<TestVector>& tv_list,
  double time
)
{
  auto& network = mgr.network();
  SizeType fault_num = mgr.total_count();
  SizeType detect_num = mgr.detected_count();
  SizeType untest_num = mgr.untestable_count();
  SizeType tv_num = tv_list.size();
  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() - network.ppi_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of total faults       = " << fault_num << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "# of test vectors       = " << tv_num << endl
       << "Total CPU time(s)       = " << (time / 1000.0) << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( stats.detect_count() > 0 ) {
    cout << endl
	 << "*** SAT instances (" << stats.detect_count() << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (stats.detect_time() / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << (stats.detect_time() / stats.detect_count()) << endl;
  }
  if ( stats.untest_count() > 0 ) {
    cout << endl
	 << "*** UNSAT instances (" << stats.untest_count() << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (stats.untest_time() / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << stats.untest_time() / stats.untest_count() << endl;
  }
  if ( stats.abort_count() > 0 ) {
    cout << endl
	 << "*** ABORT instances ***" << endl
	 << "  " << setw(10) << stats.abort_count()
	 << "  " << stats.abort_time()
	 << "  " << setw(8) << stats.abort_time() / stats.abort_count() << endl;
  }

  cout << endl
       << "SAT statistics" << endl
       << endl
       << "CNF generation" << endl
       << "  " << setw(10) << stats.cnfgen_count()
       << "  " << (stats.cnfgen_time() / 1000.0)
       << "  " << setw(8) << stats.cnfgen_time() / stats.cnfgen_count()
       << endl
       << endl
       << "# of restarts (Ave./Max)       = "
       << setw(10) << (double) stats.sat_stats().mRestart / stats.total_count()
       << " / " << setw(8) << stats.sat_stats_max().mRestart << endl

       << "# of conflicts (Ave./Max)      = "
       << setw(10) << (double) stats.sat_stats().mConflictNum / stats.total_count()
       << " / " << setw(8) << stats.sat_stats_max().mConflictNum << endl

       << "# of decisions (Ave./Max)      = "
       << setw(10) << (double) stats.sat_stats().mDecisionNum / stats.total_count()
       << " / " << setw(8) << stats.sat_stats_max().mDecisionNum << endl

       << "# of implications (Ave./Max)   = "
       << setw(10) << (double) stats.sat_stats().mPropagationNum / stats.total_count()
       << " / " << setw(8) << stats.sat_stats_max().mPropagationNum << endl;

  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << (stats.backtrace_time() / 1000.0)
       << "  " << setw(8) << stats.backtrace_time() / stats.detect_count() << endl;
  cout.flags(save);
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
  int cube_per_fault = 300;
  bool debug_fault_reduce = false;
  bool debug_testcube_gen = false;

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
      else if ( strcmp(argv[pos], "--cube-per-fault") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  cube_per_fault = atoi(argv[pos]);
	}
	else {
	  cerr << "--cube-per-fault requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--debug-all") == 0 ) {
	debug_fault_reduce = true;
	debug_testcube_gen = true;
      }
      else if ( strcmp(argv[pos], "--debug-fault_reduce") == 0 ) {
	debug_fault_reduce = true;
      }
      else if ( strcmp(argv[pos], "--debug-testcube_gen") == 0 ) {
	debug_testcube_gen = true;
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
  JsonValue option{option_dict};

  auto fault_list = network.rep_fault_list();

  Timer total_timer;
  total_timer.start();

  Timer timer;
  timer.start();

  DtpgMgr mgr{network, fault_list};

  vector<pair<const TpgFault*, TestVector>> ErrorList;
  auto stats = mgr.run(
    // detected callback
    [&](DtpgMgr& mgr, const TpgFault* f, TestVector tv) { },
    // untestable callback
    [&](DtpgMgr&, const TpgFault* f) { },
    // abort callback
    [&](DtpgMgr&, const TpgFault* f) { },
    option);

  timer.stop();
  auto time = timer.get_time();

  if ( verbose ) {
    print_stats(mgr, stats, mgr.testvector_list(), time);
  }

  timer.reset();
  timer.start();

  vector<const TpgFault*> det_fault_list;
  det_fault_list.reserve(mgr.detected_count());
  for ( auto f: fault_list ) {
    if ( mgr.dtpg_result(f).status() == FaultStatus::Detected ) {
      det_fault_list.push_back(f);
    }
  }

  unordered_map<string, JsonValue> fr_option_dict;
  if ( debug_fault_reduce ) {
    fr_option_dict.emplace("debug", JsonValue{true});
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    fr_option_dict.emplace("sat_param", sat_obj);
  }
  if ( cube_per_fault > 1 ) {
    fr_option_dict.emplace("cube_per_fault", JsonValue{cube_per_fault});
  }
  fr_option_dict.emplace("loop_limit", JsonValue{loop});
  JsonValue fr_option{fr_option_dict};
  TestCubeGen fr{network, fr_option};

  vector<TestCube> cube_list;
  auto reduced_fault_list = fr.run(det_fault_list, mgr.testvector_list(),
				   cube_list);

  timer.stop();

  cout << "=========================================" << endl
       << "Detected Faults:     " << det_fault_list.size() << endl
       << "Reduced Faults:      " << reduced_fault_list.size() << endl
       << "Total # of cubes:    " << cube_list.size() << endl
       << "CPU time:            " << timer.get_time() << endl;

  timer.reset();
  timer.start();

  FaultGroupGen fgg{network, fr_option};
  auto fg_list = fgg.generate(reduced_fault_list, cube_list);

  timer.stop();
  total_timer.stop();

  cout << "=========================================" << endl
       << "# of patterns        " << fg_list.size() << endl
       << "CPU time:            " << timer.get_time() << endl
       << "=========================================" << endl
       << "Total CPU time:      " << total_timer.get_time() << endl;


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
