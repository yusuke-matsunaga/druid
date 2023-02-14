
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgMgr.h"
#include "TpgFault.h"
#include "DopVerifyResult.h"
#include "ym/SatSolverType.h"
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
  SizeType detect_num,
  SizeType untest_num,
  double time,
  const DtpgStats& stats
)
{
  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() - network.ppi_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of total faults       = " << network.rep_fault_num() << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "Total CPU time(s)       = " << (time / 1000.0) << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( stats.mCnfGenCount > 0 ) {
    cout << "CNF generation" << endl
	 << "  " << setw(10) << stats.mCnfGenCount
	 << "  " << (stats.mCnfGenTime / 1000.0)
	 << "  " << setw(8) << stats.mCnfGenTime / stats.mCnfGenCount
	 <<  endl;
  }
  if ( stats.mDetCount > 0 ) {
    cout << endl
	 << "*** SAT instances (" << stats.mDetCount << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (stats.mDetTime / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << (stats.mDetTime / stats.mDetCount) << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) stats.mDetStats.mRestart / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) stats.mDetStats.mConflictNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) stats.mDetStats.mDecisionNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) stats.mDetStats.mPropagationNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mPropagationNum << endl;
  }
  if ( stats.mRedCount > 0 ) {
    cout << endl
	 << "*** UNSAT instances (" << stats.mRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = "
	 << setw(10) << (stats.mRedTime / 1000.0) << endl
	 << "Average CPU time (ms)          = "
	 << setw(10) << stats.mRedTime / stats.mRedCount << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) stats.mRedStats.mRestart / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) stats.mRedStats.mConflictNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) stats.mRedStats.mDecisionNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) stats.mRedStats.mPropagationNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mPropagationNum << endl;
  }
  if ( stats.mAbortCount > 0 ) {
    cout << endl
	 << "*** ABORT instances ***" << endl
	 << "  " << setw(10) << stats.mAbortCount
	 << "  " << stats.mAbortTime
	 << "  " << setw(8) << stats.mAbortTime / stats.mAbortCount << endl;
  }
  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << (stats.mBackTraceTime / 1000.0)
       << "  " << setw(8) << stats.mBackTraceTime / stats.mDetCount << endl;
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
    print_network(cout, network);
  }

  SatSolverType solver_type{sat_type, sat_option, sat_outp};

  TpgMgr mgr{network, fault_type, mode, just_type, solver_type};

  DopVerifyResult verify_result;
  mgr.add_verify_dop(verify_result);

  Timer timer;
  timer.start();

  mgr.run();

  timer.stop();
  auto time = timer.get_time();

  if ( verbose ) {
    print_stats(network, mgr.detect_count(), mgr.untest_count(),
		time, mgr.dtpg_stats());
  }

  SizeType n = verify_result.error_count();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto f = verify_result.error_fault(i);
    auto tv = verify_result.error_testvector(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << tv << endl;
  }

  return n;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}