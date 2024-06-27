
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "DtpgMgr.h"
#include "TpgFault.h"
#include "ym/SatInitParam.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?--mffc? --blif|--iscas89 <file>" << endl;
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
dtpg_test(
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
  string mode{};
  string driver{};
  bool drop = false;
  bool fix = false;
  bool multi = false;
  bool dump = false;
  bool verbose = false;
  string sat_log;
  bool show_untestable_faults = false;
  string just_type;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--ffr") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--ffr and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "ffr";
      }
      else if ( strcmp(argv[pos], "--mffc") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--mffc and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "mffc";
      }
      else if ( strcmp(argv[pos], "--node") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--node and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "node";
      }
      else if ( strcmp(argv[pos], "--engine") == 0 ) {
	if ( driver != string{} ) {
	  cerr << "--engine  and --" << driver << " are mutually exclusive" << endl;
	  return -1;
	}
	driver = "engine";
      }
      else if ( strcmp(argv[pos], "--struct_enc") == 0 ) {
	if ( driver != string{} ) {
	  cerr << "--struct_enc and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	driver = "struct_enc";
      }
      else if ( strcmp(argv[pos], "--enc") == 0 ) {
	if ( driver != string{} ) {
	  cerr << "--enc and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	driver = "enc";
      }
      else if ( strcmp(argv[pos], "--enc2") == 0 ) {
	if ( driver != string{} ) {
	  cerr << "--enc2 and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	driver = "enc2";
      }
      else if ( strcmp(argv[pos], "--sat_type") == 0 ) {
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
      else if ( strcmp(argv[pos], "--just_naive") == 0 ) {
	if ( just_type != "" ) {
	  cerr << "--just_naive and " << just_type << " are mutually exclusive" << endl;
	  return -1;
	}
	just_type = "naive";
      }
      else if ( strcmp(argv[pos], "--just1") == 0 ) {
	if ( just_type != "" ) {
	  cerr << "--just1 and " << just_type << " are mutually exclusive" << endl;
	  return -1;
	}
	just_type = "just1";
      }
      else if ( strcmp(argv[pos], "--just2") == 0 ) {
	if ( just_type != "" ) {
	  cerr << "--just2 and " << just_type << " are mutually exclusive" << endl;
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
	  cerr << "--sat_log requires <string> argument" << endl;
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

  if ( mode == string{} ) {
    // ffr をデフォルトにする．
    mode = "ffr";
  }
  if ( driver == string{} ) {
    // engine をデフォルトにする．
    driver = "engine";
  }

  if ( !sa_mode && !td_mode ) {
    // sa_mode をデフォルトにする．
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  if ( dump ) {
    network.print(cout);
  }

  unordered_map<string, JsonValue> option_dict;
  option_dict.emplace("group_mode", mode);
  option_dict.emplace("driver_type", driver);
  if ( just_type != string{} ) {
    option_dict.emplace("justifier", just_type);
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    if ( sat_log != string{} ) {
      unordered_map<string, JsonValue> log_option_dict;
      if ( sat_log == "stdout" ) {
	log_option_dict.emplace("stdout", JsonValue{true});
      }
      else if ( sat_log == "stderr" ) {
	log_option_dict.emplace("stderr", JsonValue{true});
      }
      else {
	log_option_dict.emplace("file", JsonValue{sat_log});
      }
      unordered_map<string, JsonValue> sat_option_dict;
      sat_option_dict.emplace("log", JsonValue{log_option_dict});
      sat_option_dict.emplace("type", sat_obj);
      sat_obj = JsonValue{sat_option_dict};
    }
    option_dict.emplace("sat_param", sat_obj);
  }
  else if ( sat_log != string{} ) {
      unordered_map<string, JsonValue> log_option_dict;
      if ( sat_log == "stdout" ) {
	log_option_dict.emplace("stdout", JsonValue{true});
      }
      else if ( sat_log == "stderr" ) {
	log_option_dict.emplace("stderr", JsonValue{true});
      }
      else {
	log_option_dict.emplace("file", JsonValue{sat_log});
      }
      unordered_map<string, JsonValue> sat_option_dict;
      sat_option_dict.emplace("log", JsonValue{log_option_dict});
      option_dict.emplace("sat_param", JsonValue{sat_option_dict});
  }
  JsonValue option{option_dict};

  auto fault_list = network.rep_fault_list();

  Timer timer;
  timer.start();

  Fsim fsim{network, fault_list, true, false};

  DtpgMgr mgr{network, fault_list};

  vector<pair<const TpgFault*, TestVector>> ErrorList;
  vector<const TpgFault*> det_fault_list;
  vector<TestVector> tv_list;
  std::mt19937 rg;
  auto stats = mgr.run(
    // detected callback
    [&](DtpgMgr& mgr, const TpgFault* f, TestVector tv) {
      det_fault_list.push_back(f);
      tv_list.push_back(tv);
      TestVector tv1{tv};
      tv1.fix_x_from_random(rg);
      DiffBits _dummy;
      bool r = fsim.spsfp(tv1, f, _dummy);
      if ( !r ) {
	ErrorList.push_back({f, tv});
      }
      fsim.set_skip(f);
      if ( drop ) {
	fsim.sppfp(tv, [&](const TpgFault* f, const DiffBits&) {
	  mgr.set_dtpg_result(f, DtpgResult::detected(tv));
	  fsim.set_skip(f);
	});
      }
    },
    // untestable callback
    [&](DtpgMgr&, const TpgFault* f) { },
    // abort callback
    [&](DtpgMgr&, const TpgFault* f) { },
    option);

  timer.stop();
  auto time = timer.get_time();

  if ( !ErrorList.empty() ) {
    cout << "Error!" << endl;
    for ( auto& p: ErrorList ) {
      auto f = p.first;
      auto tv = p.second;
      cout << f->str() << ": " << tv.hex_str() << endl;
    }
  }

  if ( verbose ) {
    print_stats(mgr, stats, tv_list, time);
  }

  if ( show_untestable_faults ) {
    cout << "Untestabel faults" << endl;
    for ( auto f: fault_list ) {
      if ( mgr.dtpg_result(f).status() == FaultStatus::Untestable ) {
	cout << f->str() << endl;
      }
    }
  }

  if ( fix ) {
    std::mt19937 randgen;
    for ( auto& tv: tv_list ) {
      TestVector fixed_tv{tv};
      fixed_tv.fix_x_from_random(randgen);
      cout << fixed_tv.hex_str() << endl;
    }
  }

  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}
