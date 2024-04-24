
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "DtpgMgr.h"
#include "TpgFault.h"
#include "TpgFaultStatusMgr.h"
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
  FaultType fault_type,
  bool blif,
  bool iscas89
)
{
  ASSERT_COND( blif | iscas89 );
  if ( blif ) {
    return TpgNetwork::read_blif(filename, fault_type);
  }
  return TpgNetwork::read_iscas89(filename, fault_type);
}

// @brief 統計情報を出力する．
void
print_stats(
  const TpgNetwork& network,
  const TpgFaultStatusMgr& fs_mgr,
  const DtpgStats& stats,
  double time
)
{
  SizeType fault_num = network.rep_fault_list().size();
  SizeType detect_num = fs_mgr.detected_count();
  SizeType untest_num = fs_mgr.untestable_count();
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

void
classify(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  FaultType fault_type,
  const vector<TestVector>& tv_list,
  bool drop,
  bool ppsfp,
  bool multi,
  const string& name
)
{
  Timer timer;
  timer.start();
  auto fault_group_list = Classifier::run(network, fault_list, tv_list, drop, ppsfp, multi);
  timer.stop();
  auto class_time = timer.get_time();

  SizeType g = 0;
  SizeType c = 0;
  for ( auto& fg: fault_group_list ) {
    SizeType n = fg.size();
    if ( n >= 2 ) {
      ++ g;
      c += n * (n - 1) / 2;
    }
  }
  cout << "# of unseparated fault group: " << g << endl;
  cout << "# of unseparated fault pair:  " << c << endl;
  cout << "Classfiy(" << name << ") time: "
       << std::fixed << std::setprecision(2)
       << (class_time / 1000) << endl;
}

void
classify2(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list,
  FaultType fault_type,
  const vector<TestVector>& tv_list,
  bool ppsfp,
  bool multi,
  const string& name
)
{
  Timer timer;
  timer.start();
  auto fault_group_list = Classifier2::run(network, fault_list, tv_list, ppsfp, multi);
  timer.stop();
  auto class_time = timer.get_time();

  SizeType g = 0;
  SizeType c = 0;
  for ( auto& fg: fault_group_list ) {
    SizeType n = fg.size();
    if ( n >= 2 ) {
      ++ g;
      c += n * (n - 1) / 2;
    }
  }
  cout << "# of unseparated fault group: " << g << endl;
  cout << "# of unseparated fault pair:  " << c << endl;
  cout << "Classfiy2(" << name << ") time: "
       << std::fixed << std::setprecision(2)
       << (class_time / 1000) << endl;
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
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  if ( dump ) {
    network.print(cout);
  }

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

  auto& rep_fault_list = network.rep_fault_list();

  Timer timer;
  timer.start();

  Fsim fsim;
  fsim.initialize(network, rep_fault_list, true, false);
  vector<pair<const TpgFault*, TestVector>> ErrorList;

  TpgFaultStatusMgr fs_mgr{rep_fault_list};
  vector<const TpgFault*> det_fault_list;
  vector<TestVector> tv_list;
  auto stats = DtpgMgr::run(network, fs_mgr,
			    [&](const TpgFault* f, TestVector tv) {
			      fs_mgr.set_status(f, FaultStatus::Detected);
			      det_fault_list.push_back(f);
			      tv_list.push_back(tv);
			      DiffBits _dummy;
			      bool r = fsim.spsfp(tv, f, _dummy);
			      if ( !r ) {
				ErrorList.push_back({f, tv});
			      }
			    },
			    [&](const TpgFault* f) {
			      fs_mgr.set_status(f, FaultStatus::Untestable);
			    },
			    [&](const TpgFault* f) {
			    },
			    option);

  timer.stop();
  auto time = timer.get_time();

  if ( verbose ) {
    print_stats(network, fs_mgr, stats, time);
  }

  for ( auto& p: ErrorList ) {
    auto f = p.first;
    auto tv = p.second;
    cout << "Error: " << f << " is not detected with "
	 << tv << endl;
  }

  timer.reset();
  timer.start();

  std::mt19937 randgen;
  vector<TestVector> fixed_tv_list;
  fixed_tv_list.reserve(tv_list.size());
  for ( auto& tv: tv_list ) {
    TestVector fixed_tv{tv};
    fixed_tv.fix_x_from_random(randgen);
    fixed_tv_list.push_back(fixed_tv);
  }

  {
    vector<const TpgFault*> fault_list2;
    Fsim fsim;
    fsim.initialize(network, det_fault_list, false, false);
    SizeType base = 0;
    vector<TestVector> tv_buf;
    for ( auto& tv: fixed_tv_list ) {
      tv_buf.push_back(tv);
      if ( tv_buf.size() == PV_BITLEN || tv_buf.size() + base == tv_list.size() ) {
	fsim.ppsfp(tv_buf, [&](
	  const TpgFault* f,
	  const DiffBitsArray& dbits
	)
	{
	  fault_list2.push_back(f);
	  fsim.set_skip(f);
	});
	base += tv_buf.size();
	tv_buf.clear();
      }
    }
    if ( fault_list2.size() != det_fault_list.size() ) {
      cout << "fault_list.size() = " << det_fault_list.size() << endl
	   << "fault_list2.size() = " << fault_list2.size() << endl;
      abort();
    }
  }

  cout << "# of faults:  " << det_fault_list.size() << endl;
  cout << "# of tv_list: " << tv_list.size() << endl;
  cout << "DTPG time:                    "
       << std::fixed << std::setprecision(2) << (time / 1000) << endl;

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   false, false, false, "no-drop, sppfp");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   false, true, false, "no-drop, ppsfp");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   true, false, false, "drop, sppfp");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   true, true, false, "drop, ppsfp");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   false, false, true, "no-drop, sppfp, multi");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   false, true, true, "no-drop, ppsfp, multi");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   true, false, true, "drop, sppfp, multi");

  classify(network, det_fault_list, fault_type, fixed_tv_list,
	   true, true, true, "drop, ppsfp multi");

#if 0
  classify2(network, det_fault_list, fault_type, fixed_tv_list,
	    false, multi, "sppfp");

  classify2(network, det_fault_list, fault_type, fixed_tv_list,
	    true, multi, "ppsfp");
#endif
  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}
