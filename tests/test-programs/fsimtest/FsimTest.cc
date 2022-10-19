
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "Fsim.h"
#include <random>
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";
bool verbose = false;

// 故障を検出したときの出力
void
print_fault(const TpgFault* f,
	    int tv_id)
{
  if ( verbose ) {
    cout << setw(7) << tv_id << ": " << f->str() << endl;
  }
}

// SPSFP のテスト
pair<int, int>
spsfp_test(
  const TpgNetwork& network,
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  vector<bool> mark(network.max_fault_id(), false);
  int det_num = 0;
  int nepat = 0;
  int i = 0;
  for ( auto tv: tv_list ) {
    bool detect = false;
    for ( auto f: network.rep_fault_list() ) {
      if ( mark[f->id()] ) {
	continue;
      }
      if ( fsim.spsfp(tv, f) ) {
	++ det_num;
	detect = true;
	mark[f->id()] = true;
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
    int n = fsim.sppfp(tv);
    if ( n > 0 ) {
      det_num += n;
      ++ nepat;
      for ( auto f: fsim.det_fault_list() ) {
	fsim.set_skip(f);
	print_fault(f, i);
      }
    }
    ++ i;
  }

  return make_pair(det_num, nepat);
}

// PPSFP のテスト
pair<int, int>
ppsfp_test(
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  int nv = tv_list.size();

  fsim.clear_patterns();
  int wpos = 0;
  int det_num = 0;
  int nepat = 0;
  for ( auto tv: tv_list ) {
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == PV_BITLEN ) {
      int n = fsim.ppsfp();

      int nb = wpos;
      PackedVal dpat_all = 0ULL;
      det_num += n;
      for ( int j = 0; j < n; ++ j ) {
	const TpgFault* f = fsim.det_fault(j);
	PackedVal dpat = fsim.det_fault_pat(j);
	fsim.set_skip(f);
	// dpat の最初の1のビットを求める．
	int first = 0;
	for ( ; first < nb; ++ first) {
	  if ( dpat & (1ULL << first) ) {
	    break;
	  }
	}
	ASSERT_COND( first < nb );
	dpat_all |= (1ULL << first);
	print_fault(f, j - wpos + first + 1);
      }
      for ( int i = 0; i < nb; ++ i ) {
	if ( dpat_all & (1ULL << i) ) {
	  ++ nepat;
	}
      }
      fsim.clear_patterns();
      wpos = 0;
    }
  }
  if ( wpos > 0 ) {
    int n = fsim.ppsfp();

    int nb = wpos;
    PackedVal dpat_all = 0ULL;
    det_num += n;
    for ( int j = 0; j < n; ++ j ) {
      const TpgFault* f = fsim.det_fault(j);
      PackedVal dpat = fsim.det_fault_pat(j);
      fsim.set_skip(f);
      // dpat の最初の1のビットを求める．
      int first = 0;
      for ( ; first < nb; ++ first) {
	if ( dpat & (1ULL << first) ) {
	  break;
	}
      }
      ASSERT_COND( first < nb );
      dpat_all |= (1ULL << first);
      print_fault(f, nv - wpos + first + 1);
    }
    for ( int i = 0; i < nb; ++ i) {
      if ( dpat_all & (1ULL << i) ) {
	++ nepat;
      }
    }
  }

  return make_pair(det_num, nepat);
}

// ランダムにテストパタンを生成する．
// @param[in] rg 乱数発生器
// @param[in] tvmgr テストベクタを管理するオブジェクト
// @param[in] nv 生成するパタン数
// @param[out] tv_list 生成されたパタンを格納するベクタ
template<class URNG>
void
randgen(
  URNG& rg,
  int input_num,
  int dff_num,
  FaultType fault_type,
  int nv,
  vector<TestVector>& tv_list
)
{
  tv_list.clear();
  tv_list.reserve(nv);
  for ( int i = 0; i < nv; ++ i ) {
    TestVector tv(input_num, dff_num, fault_type);
    tv.set_from_random(rg);
    tv_list.push_back(std::move(tv));
  }
}

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?-n #pat? ?--fsim2|--fsim3? ?--ppspf|--sppfp? --blif|--iscas89 <file>" << endl;
}

int
fsim2test(
  int argc,
  char** argv
)
{
  int npat = 0;
  bool blif = false;
  bool iscas89 = false;

  bool fsim2 = false;
  bool fsim3 = false;

  bool ppsfp = false;
  bool sppfp = false;

  bool sa_mode = false;
  bool td_mode = false;

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
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	if ( iscas89 ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	blif = true;
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	if ( blif ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	iscas89 = true;
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

  if ( !blif && !iscas89 ) {
    // とりあえず blif をデフォルトにする．
    blif = true;
  }

  string filename = argv[pos];
  TpgNetwork network;
  if ( blif ) {
    if ( !network.read_blif(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else if ( iscas89 ) {
    if ( !network.read_iscas89(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else {
    ASSERT_NOT_REACHED;
  }

  if ( !sa_mode && !td_mode ) {
    sa_mode = true;
  }
  if ( td_mode && network.dff_num() == 0 ) {
    cerr << "Network(" << filename << ") is not sequential,"
	 << " --transition-delay option is ignored." << endl;
    td_mode = false;
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  Fsim fsim{network, fault_type, fsim3};

  std::mt19937 rg;
  vector<TestVector> tv_list;

  int input_num = network.input_num();
  int dff_num = network.dff_num();
  randgen(rg, input_num, dff_num, fault_type, npat, tv_list);

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
    dpnum = spsfp_test(network, fsim, tv_list);
  }

  int det_num = dpnum.first;
  int nepat = dpnum.second;

  timer.stop();
  auto time = timer.get_time();

  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of simulated patterns = " << npat << endl
       << "# of effective patterns = " << nepat << endl
       << "# of total faults       = " << network.rep_fault_num() << endl
       << "# of detected faults    = " << det_num << endl
       << "# of undetected faults  = " << network.rep_fault_num() - det_num << endl
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
