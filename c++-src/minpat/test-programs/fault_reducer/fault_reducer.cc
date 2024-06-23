
/// @file fault_reducer.cc
/// @brief FaultReducer を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "DtpgMgr.h"
#include "TpgFault.h"
#include "FaultReducer.h"
#include "NaiveDomChecker.h"
#include "StructDomChecker.h"
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
  bool se = false;
  bool verbose = false;
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
      else if ( strcmp(argv[pos], "--struct_enc") == 0 ) {
	se = true;
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
  JsonValue option{option_dict};

  auto fault_list = network.rep_fault_list();

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

  vector<const TpgFault*> det_fault_list;
  det_fault_list.reserve(mgr.detected_count());
  for ( auto f: fault_list ) {
    if ( mgr.dtpg_result(f).status() == FaultStatus::Detected ) {
      det_fault_list.push_back(f);
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
	  cout << f2->str() << endl;
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
  else if ( se ) {
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
	StructDomChecker checker{network, f1, f2, option};
	if ( checker.check() ) {
	  {
	    cout << endl;
	    network.print(cout);
	    cout << endl;
	  }
	  cout << f1->str() << " ==> " << f2->str() << endl;
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
    JsonValue fr_option{fr_option_dict};
    FaultReducer fr{network, det_fault_list, mgr.testvector_list(), fr_option};

    auto reduced_fault_list = fr.run();

    cout << "Detected Faults: " << det_fault_list.size() << endl
	 << "Reduced Faults:  " << reduced_fault_list.size() << endl;
  }

  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::fault_reducer(argc, argv);
}
