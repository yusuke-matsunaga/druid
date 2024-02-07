
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include <random>
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";
bool verbose = false;

// 故障を検出したときの出力
void
print_fault(
  const TpgFault& f,
  int tv_id
)
{
  if ( verbose ) {
    cout << setw(7) << tv_id << ": " << f << endl;
  }
}

// SPSFP のテスト
pair<int, int>
spsfp_test(
  const TpgNetwork& network,
  TpgFaultMgr& fmgr,
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  int det_num = 0;
  int nepat = 0;
  int i = 0;
  for ( auto tv: tv_list ) {
    bool detect = false;
    for ( auto f: fmgr.rep_fault_list() ) {
      if ( fmgr.get_status(f) == FaultStatus::Detected ) {
	continue;
      }
      DiffBits dbits;
      if ( fsim.spsfp(tv, f, dbits) ) {
	++ det_num;
	detect = true;
	fmgr.set_status(f, FaultStatus::Detected);
	print_fault(f, i);
      }
    }
    if ( detect ) {
      ++ nepat;
    }
    ++ i;
  }

  return make_pair(det_num, nepat);
}

// SPPFP のテスト
pair<int, int>
sppfp_test(
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  int det_num = 0;
  int nepat = 0;
  int i = 0;
  for ( auto tv: tv_list ) {
    bool drop = false;
    fsim.sppfp(tv,
	       [&](
		 SizeType,
		 TpgFault f,
		 DiffBits dbits
	       )
	       {
		 ++ det_num;
		 drop = true;
		 fsim.set_skip(f);
		 print_fault(f, i);
	       });
    if ( drop ) {
      ++ nepat;
    }
    ++ i;
  }

  return make_pair(det_num, nepat);
}

// PPSFP のテスト
pair<SizeType, SizeType>
ppsfp_test(
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  int nv = tv_list.size();

  SizeType nepat = 0;
  SizeType det_num = 0;
  unordered_set<SizeType> pat_dict;
  fsim.ppsfp(tv_list,
	     [&](
	       SizeType index,
	       TpgFault f,
	       DiffBits dbits
	     )
	     {
	       if ( !fsim.get_skip(f) ) {
		 fsim.set_skip(f);
		 ++ det_num;
		 if ( pat_dict.count(index) == 0 ) {
		   pat_dict.emplace(index);
		   ++ nepat;
		 }
		 print_fault(f, index);
	       }
	     });
  return make_pair(det_num, nepat);
}

// ランダムにテストパタンを生成する．
template<class URNG>
void
randgen(
  URNG& rg,
  int input_num,
  int dff_num,
  bool has_prev_state,
  int nv,
  vector<TestVector>& tv_list
)
{
  tv_list.clear();
  tv_list.reserve(nv);
  for ( int i = 0; i < nv; ++ i ) {
    TestVector tv(input_num, dff_num, has_prev_state);
    tv.set_from_random(rg);
    tv_list.push_back(std::move(tv));
  }
}

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?-n #pat? ?--fsim2|--fsim3? ?--ppspf|--sppfp? --blif|--iscas89 <file>" << endl;
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

int
fsim2test(
  int argc,
  char** argv
)
{
  int npat = 0;
  string format = "blif";

  bool fsim2 = false;
  bool fsim3 = false;

  bool ppsfp = false;
  bool sppfp = false;

  bool sa_mode = false;
  bool td_mode = false;

  bool old = false;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "-n") == 0 ) {
	++ pos;
	if ( pos >= argc ) {
	  cerr << " -n option requires #pat" << endl;
	  return -1;
	}
	npat = atoi(argv[pos]);
	if ( npat == 0 ) {
	  cerr << " integer expected after -n" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--fsim2") == 0 ) {
	if ( fsim3 ) {
	  cerr << "--fsim2 and --fsim3 are mutually exclusive" << endl;
	  return -1;
	}
	fsim2 = true;
      }
      else if ( strcmp(argv[pos], "--fsim3") == 0 ) {
	if ( fsim2 ) {
	  cerr << "--fsim2 and --fsim3 are mutually exclusive" << endl;
	  return -1;
	}
	fsim3 = true;
      }
      else if ( strcmp(argv[pos], "--ppsfp") == 0 ) {
	if ( sppfp ) {
	  cerr << "--ppspf and --sppfp are mutually exclusive" << endl;
	  return -1;
	}
	ppsfp = true;
      }
      else if ( strcmp(argv[pos], "--sppfp") == 0 ) {
	if ( ppsfp ) {
	  cerr << "--ppspf and --sppfp are mutually exclusive" << endl;
	  return -1;
	}
	sppfp = true;
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
      else if ( strcmp(argv[pos], "--old") == 0 ) {
	old = true;
      }
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	format = "blif";
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	format = "iscas89";
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

  if ( npat == 0 ) {
    // デフォルトのパタン数を設定する．
    npat = 100000;
  }

  if ( !fsim2 && !fsim3 ) {
    // fsim2 をデフォルトにする．
    fsim2 = true;
  }

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format);

  FaultType fault_type = FaultType::None;
  if ( !sa_mode && !td_mode ) {
    sa_mode = true;
    fault_type = FaultType::StuckAt;
  }
  if ( td_mode && network.dff_num() == 0 ) {
    cerr << "Network(" << filename << ") is not sequential,"
	 << " --transition-delay option is ignored." << endl;
    td_mode = false;
    sa_mode = true;
    fault_type = FaultType::TransitionDelay;
  }
  bool has_prev_state = td_mode;

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(network, fault_type);

  bool prev_state = fault_type == FaultType::TransitionDelay;
  Fsim fsim;
  if ( old ) {
    fsim.initialize_naive(network, prev_state, fsim3);
  }
  else {
    fsim.initialize(network, prev_state, fsim3);
  }

  fsim.set_fault_list(fmgr.rep_fault_list());

  std::mt19937 rg;
  vector<TestVector> tv_list;

  int input_num = network.input_num();
  int dff_num = network.dff_num();
  randgen(rg, input_num, dff_num, has_prev_state, npat, tv_list);

  Timer timer;
  timer.start();

  pair<int, int> dpnum;
  if ( ppsfp ) {
    dpnum = ppsfp_test(fsim, tv_list);
  }
  else if ( sppfp ) {
    dpnum = sppfp_test(fsim, tv_list);
  }
  else {
    // デフォルトフォールバックは SPSFP
    dpnum = spsfp_test(network, fmgr, fsim, tv_list);
  }

  int det_num = dpnum.first;
  int nepat = dpnum.second;

  timer.stop();
  auto time = timer.get_time();

  SizeType nf = fmgr.rep_fault_list().size();
  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of simulated patterns = " << npat << endl
       << "# of effective patterns = " << nepat << endl
       << "# of total faults       = " << nf << endl
       << "# of detected faults    = " << det_num << endl
       << "# of undetected faults  = " << nf - det_num << endl
       << "Total CPU time          = " << time << endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::fsim2test(argc, argv);
}
