
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2024 Yusuke Matsunaga
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
print_fault(
  const TpgFault* f,
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
  const vector<const TpgFault*>& fault_list,
  Fsim& fsim,
  const vector<TestVector>& tv_list
)
{
  SizeType det_num = 0;
  SizeType nepat = 0;
  SizeType i = 0;
  unordered_set<SizeType> det_mark;
  for ( auto tv: tv_list ) {
    bool detect = false;
    for ( auto f: fault_list ) {
      if ( det_mark.count(f->id()) > 0 ) {
	continue;
      }
      DiffBits dbits;
      if ( fsim.spsfp(tv, f, dbits) ) {
	++ det_num;
	detect = true;
	det_mark.emplace(f->id());
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
  const vector<TestVector>& tv_list,
  SizeType max_fid,
  bool drop
)
{
  int det_num = 0;
  int nepat = 0;
  int i = 0;
  std::mutex mtx;
  vector<bool> det_array(max_fid, false);
  for ( auto tv: tv_list ) {
    bool detected = false;
    fsim.sppfp(tv,
	       [&](
		 const TpgFault* f,
		 const DiffBits& dbits
	       )
	       {
		 if ( !det_array[f->id()] ) {
		   det_array[f->id()] = true;
		   ++ det_num;
		   if ( drop ) {
		     fsim.set_skip(f);
		   }
		   print_fault(f, i);
		   detected = true;
		 }
	       });
    if ( detected ) {
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
  const vector<TestVector>& tv_list,
  SizeType max_fid,
  bool drop
)
{
  SizeType nepat = 0;
  SizeType det_num = 0;
  std::mutex mtx;
  unordered_set<SizeType> pat_dict;
  vector<bool> det_array(max_fid, false);
  vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType NV = tv_list.size();
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    if ( tv_buff.size() == PV_BITLEN || tv_buff.size() + base == NV )  {
      fsim.ppsfp(tv_buff,
		 [&](
		   const TpgFault* f,
		   const DiffBitsArray& dbits_array
		 )
		 {
		   if ( !det_array[f->id()] ) {
		     det_array[f->id()] = true;
		     ++ det_num;
		     if ( drop ) {
		       fsim.set_skip(f);
		     }
		     auto obs = dbits_array.dbits_union();
		     for ( SizeType lindex = 0; lindex < tv_buff.size(); ++ lindex ) {
		       if ( obs & (1UL << lindex) ) {
			 SizeType index = base + index;
			 if ( pat_dict.count(index) == 0 ) {
			   pat_dict.emplace(index);
			   ++ nepat;
			 }
			 print_fault(f, index);
		       }
		     }
		   }
		 });
      base += tv_buff.size();
      tv_buff.clear();
    }
  }
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

  bool drop = false;

  bool multi = false;

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
      else if ( strcmp(argv[pos], "--drop") == 0 ) {
	drop = true;
      }
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
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

  FaultType fault_type = FaultType::None;
  if ( !sa_mode && !td_mode ) {
    sa_mode = true;
    fault_type = FaultType::StuckAt;
  }
  if ( td_mode ) {
    fault_type = FaultType::TransitionDelay;
  }

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);
  auto& fault_list = network.rep_fault_list();

  Fsim fsim{network, fault_list, fsim3, multi};

  SizeType max_fid = 0;
  for ( auto f: fault_list ) {
    max_fid = std::max(max_fid, f->id());
  }
  ++ max_fid;

  std::mt19937 rg;
  vector<TestVector> tv_list;

  int input_num = network.input_num();
  int dff_num = network.dff_num();
  bool has_prev_state = td_mode;
  randgen(rg, input_num, dff_num, has_prev_state, npat, tv_list);

  Timer timer;
  timer.start();

  pair<int, int> dpnum;
  if ( ppsfp ) {
    dpnum = ppsfp_test(fsim, tv_list, max_fid, drop);
  }
  else if ( sppfp ) {
    dpnum = sppfp_test(fsim, tv_list, max_fid, drop);
  }
  else {
    // デフォルトフォールバックは SPSFP
    dpnum = spsfp_test(network, fault_list, fsim, tv_list);
  }

  int det_num = dpnum.first;
  int nepat = dpnum.second;

  timer.stop();
  auto time = timer.get_time();

  SizeType nf = fault_list.size();
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
