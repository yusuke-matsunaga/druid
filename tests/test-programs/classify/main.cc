
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "DtpgMgr.h"
#include "TpgFault.h"
#include "DopVerifyResult.h"
#include "Classifier.h"
#include "Classifier2.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?--mffc? --blif|--iscas89 <file>" << endl;
}

TpgNetwork
read_network(
  const string& filename,
  bool blif,
  bool iscas89
)
{
  ASSERT_COND( blif | iscas89 );
  if ( blif ) {
    return TpgNetwork::read_blif(filename);
  }
  return TpgNetwork::read_iscas89(filename);
}

// @brief 統計情報を出力する．
void
print_stats(
  const TpgNetwork& network,
  DtpgMgr& mgr,
  double time
)
{
  SizeType fault_num = mgr.fault_num();
  SizeType detect_num = mgr.detect_count();
  SizeType untest_num = mgr.untest_count();
  const DtpgStats& stats = mgr.dtpg_stats();
  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() - network.ppi_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of total faults       = " << fault_num << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
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

  bool dump = false;

  bool verbose = false;

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
      else if ( strcmp(argv[pos], "--ffr_se") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--ffr_se  and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "ffr_se";
      }
      else if ( strcmp(argv[pos], "--mffc_se") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--mffc_new and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "mffc_se";
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
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
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

  if ( mode == string{} ) {
    // ffr をデフォルトにする．
    mode = "ffr";
  }

  if ( !sa_mode && !td_mode ) {
    // sa_mode をデフォルトにする．
    sa_mode = true;
  }

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format);

  if ( td_mode && network.dff_num() == 0 ) {
    cerr << "Network is combinational, stuck-at mode is assumed" << endl;
    td_mode = false;
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  if ( dump ) {
    network.print(cout);
  }

  TpgFaultMgr fault_mgr;
  fault_mgr.gen_fault_list(network, fault_type);

  unordered_map<string, JsonValue> option_dict;
  option_dict.emplace("dtpg_type", mode);
  option_dict.emplace("just_type", just_type);
  vector<JsonValue> dop_list;
  dop_list.push_back(JsonValue{"base"});
  dop_list.push_back(JsonValue{"verify"});
  dop_list.push_back(JsonValue{"drop"});
  dop_list.push_back(JsonValue{"tvlist"});
  option_dict.emplace("dop", JsonValue{dop_list});
  option_dict.emplace("uop", JsonValue{"base"});
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue::parse(sat_type);
    option_dict.emplace("sat_param", sat_obj);
  }
  JsonValue option{option_dict};

  DtpgMgr mgr{network, fault_mgr, option};

  Timer timer;
  timer.start();

  mgr.run();

  timer.stop();
  auto time = timer.get_time();

  if ( verbose ) {
    print_stats(network, mgr, time);
  }

  auto& verify_result = mgr.verify_result();
  SizeType n = verify_result.error_count();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto f = verify_result.error_fault(i);
    auto tv = verify_result.error_testvector(i);
    cout << "Error: " << f << " is not detected with "
	 << tv << endl;
  }

  vector<TpgFault> fault_list;
  //cout << "Untestable faults:" << endl;
  for ( auto f: fault_mgr.rep_fault_list() ) {
    if ( fault_mgr.get_status(f) == FaultStatus::Detected ) {
      fault_list.push_back(f);
    }
    if ( fault_mgr.get_status(f) == FaultStatus::Untestable ) {
      //cout << f << endl;
    }
  }
  //cout << "====================" << endl;

  timer.reset();
  timer.start();

  std::mt19937 randgen;
  auto& tv_list = mgr.tv_list();
  vector<TestVector> fixed_tv_list;
  fixed_tv_list.reserve(tv_list.size());
  for ( auto& tv: tv_list ) {
    TestVector fixed_tv{tv};
    fixed_tv.fix_x_from_random(randgen);
    fixed_tv_list.push_back(fixed_tv);
  }

  cout << "# of faults:  " << fault_list.size() << endl;
  cout << "# of tv_list: " << tv_list.size() << endl;
  cout << "DTPG time:                    "
       << std::fixed << std::setprecision(2) << (time / 1000) << endl;

  Classifier cls{network, fault_list, td_mode};

  auto fault_group_list = cls.run(fixed_tv_list, true);

  timer.stop();
  auto class_time = timer.get_time();

  SizeType g = 0;
  SizeType c = 0;
  for ( auto fg: fault_group_list ) {
#if 0
    for ( auto f: fg ) {
      cout << " " << f;
    }
    cout << endl;
#endif
    SizeType n = fg.size();
    if ( n >= 2 ) {
      ++ g;
      c += n * (n - 1) / 2;
    }
  }
  cout << "# of unseparated fault group: " << g << endl;
  cout << "# of unseparated fault pair:  " << c << endl;
  cout << "Classify time:                "
       << std::fixed << std::setprecision(2) << (class_time / 1000 ) << endl
       << std::defaultfloat;

  timer.reset();
  timer.start();

#if 0
  Classifier cls2{network, fault_list, td_mode};

  vector<vector<vector<TpgFault>>> fg_list_array;
  fg_list_array.reserve(tv_list.size());
  for ( auto& tv: tv_list ) {
    fg_list_array.push_back(cls2.run(tv));
  }
#endif

  Classifier cls2{network, fault_list, td_mode};
  auto fault_group_list2 = cls2.run(fixed_tv_list, false);

  timer.stop();
  auto class2_time = timer.get_time();

  cout << "Classfiy2 time:               "
       << std::fixed << std::setprecision(2)
       << (class2_time / 1000) << endl;

  timer.reset();
  timer.start();

  Classifier2 cls3{network, fault_list, td_mode};
  auto fault_group_list3 = cls3.run(fixed_tv_list);

  timer.stop();
  auto class3_time = timer.get_time();

  cout << "Classfiy3 time:               "
       << std::fixed << std::setprecision(2)
       << (class3_time / 1000) << endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}
