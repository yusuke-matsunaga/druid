
/// @file FsimTest.cc
/// @brief FsimTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"
#include <random>
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";
bool verbose = false;

// 故障を検出したときの出力
void
print_fault(
  TpgFault f,
  int tv_id
)
{
  if ( verbose ) {
    std::cout << std::setw(7) << tv_id << ": " << f
	      << std::endl;
  }
}

// SPSFP のテスト
std::pair<int, int>
spsfp_test(
  const TpgNetwork& network,
  const TpgFaultList& fault_list,
  Fsim& fsim,
  const std::vector<TestVector>& tv_list
)
{
  SizeType det_num = 0;
  SizeType nepat = 0;
  SizeType i = 0;
  std::unordered_set<SizeType> det_mark;
  for ( auto tv: tv_list ) {
    bool detect = false;
    for ( auto fault: fault_list ) {
      if ( det_mark.count(fault.id()) > 0 ) {
	continue;
      }
      DiffBits dbits;
      if ( fsim.spsfp(tv, fault, dbits) ) {
	++ det_num;
	detect = true;
	det_mark.emplace(fault.id());
	print_fault(fault, i);
      }
    }
    if ( detect ) {
      ++ nepat;
    }
    ++ i;
  }

  return std::make_pair(det_num, nepat);
}

// SPPFP のテスト
std::pair<int, int>
sppfp_test(
  const TpgNetwork& network,
  Fsim& fsim,
  const std::vector<TestVector>& tv_list,
  SizeType max_fid,
  bool drop
)
{
  int det_num = 0;
  int nepat = 0;
  int i = 0;

  std::vector<bool> det_array(max_fid, false);
  for ( auto tv: tv_list ) {
    bool detected = false;
    auto res = fsim.sppfp(tv);
    if ( res.tv_num() != 1 ) {
      throw std::logic_error{"something wrong"};
    }
    for ( auto fid: res.fault_list(0) ) {
      auto dbits = res.diffbits(0, fid);
      if ( !det_array[fid] ) {
	det_array[fid] = true;
	++ det_num;
	auto fault = network.fault(fid);
	if ( drop ) {
	  fsim.set_skip(fault);
	}
	print_fault(fault, i);
	detected = true;
      }
    }
    if ( detected ) {
      ++ nepat;
    }
    ++ i;
  }

  return std::make_pair(det_num, nepat);
}

// PPSFP のテスト
std::pair<SizeType, SizeType>
ppsfp_test(
  const TpgNetwork& network,
  Fsim& fsim,
  const std::vector<TestVector>& tv_list,
  SizeType max_fid,
  bool drop
)
{
  SizeType nepat = 0;
  SizeType det_num = 0;

  std::unordered_set<SizeType> pat_dict;
  std::vector<bool> det_array(max_fid, false);
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType NV = tv_list.size();
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    if ( tv_buff.size() == PV_BITLEN || tv_buff.size() + base == NV )  {
      auto res = fsim.ppsfp(tv_buff);
      for ( SizeType tv_id = 0; tv_id < tv_buff.size(); ++ tv_id ) {
	for ( auto fid: res.fault_list(tv_id) ) {
	  auto dbits = res.diffbits(tv_id, fid);
	  det_array[fid] = true;
	  ++ det_num;
	  auto fault = network.fault(fid);
	  if ( drop ) {
	    fsim.set_skip(fault);
	  }
	  auto index = base + tv_id;
	  if ( pat_dict.count(index) == 0 ) {
	    pat_dict.emplace(index);
	    ++ nepat;
	  }
	  print_fault(fault, index);
	}
      }
      base += tv_buff.size();
      tv_buff.clear();
    }
  }
  return std::make_pair(det_num, nepat);
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
  std::vector<TestVector>& tv_list
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
  std::cerr << "USAGE: " << argv0
	    << " ?-n #pat? ?--fsim2|--fsim3? ?--ppspf|--sppfp? --blif|--iscas89 <file>"
	    << std::endl;
}

int
fsim2test(
  int argc,
  char** argv
)
{
  int npat = 0;
  std::string format = "blif";

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
	  std::cerr << " -n option requires #pat"
		    << std::endl;
	  return -1;
	}
	npat = atoi(argv[pos]);
	if ( npat == 0 ) {
	  std::cerr << " integer expected after -n"
		    << std::endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--fsim2") == 0 ) {
	if ( fsim3 ) {
	  std::cerr << "--fsim2 and --fsim3 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	fsim2 = true;
      }
      else if ( strcmp(argv[pos], "--fsim3") == 0 ) {
	if ( fsim2 ) {
	  std::cerr << "--fsim2 and --fsim3 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	fsim3 = true;
      }
      else if ( strcmp(argv[pos], "--ppsfp") == 0 ) {
	if ( sppfp ) {
	  std::cerr << "--ppspf and --sppfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	ppsfp = true;
      }
      else if ( strcmp(argv[pos], "--sppfp") == 0 ) {
	if ( ppsfp ) {
	  std::cerr << "--ppspf and --sppfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sppfp = true;
      }
      else if ( strcmp(argv[pos], "--stuck-at") == 0 ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( strcmp(argv[pos], "--transition-delay") == 0 ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
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
	std::cerr << argv[pos] << ": illegal option"
		  << std::endl;
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

  std::string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);
  auto fault_list = network.rep_fault_list();

  auto option = JsonValue::object();
  option.add("has_x", fsim3);
  option.add("multi_thread", multi);
  auto fsim = Fsim(network, fault_list, option);

  SizeType max_fid = fault_list.max_fid();
  ++ max_fid;

  std::mt19937 rg;
  std::vector<TestVector> tv_list;

  int input_num = network.input_num();
  int dff_num = network.dff_num();
  bool has_prev_state = td_mode;
  randgen(rg, input_num, dff_num, has_prev_state, npat, tv_list);

  Timer timer;
  timer.start();

  std::pair<int, int> dpnum;
  if ( ppsfp ) {
    dpnum = ppsfp_test(network, fsim, tv_list, max_fid, drop);
  }
  else if ( sppfp ) {
    dpnum = sppfp_test(network, fsim, tv_list, max_fid, drop);
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
  std::cout << "# of inputs             = " << network.input_num() << std::endl
	    << "# of outputs            = " << network.output_num() << std::endl
	    << "# of DFFs               = " << network.dff_num() << std::endl
	    << "# of logic gates        = " << network.node_num() << std::endl
	    << "# of MFFCs              = " << network.mffc_num() << std::endl
	    << "# of FFRs               = " << network.ffr_num() << std::endl
	    << "# of simulated patterns = " << npat << std::endl
	    << "# of effective patterns = " << nepat << std::endl
	    << "# of total faults       = " << nf << std::endl
	    << "# of detected faults    = " << det_num << std::endl
	    << "# of undetected faults  = " << nf - det_num << std::endl
	    << "Total CPU time          = " << time << std::endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::fsim2test(argc, argv);
}
