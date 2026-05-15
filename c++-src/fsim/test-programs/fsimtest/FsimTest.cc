
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
      if ( fsim.spsfp(tv, fault) ) {
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

TpgFaultList
simulate1(
  Fsim& fsim,
  const TestVector& tv,
  bool ppsfp
)
{
  if ( ppsfp ) {
    auto det_list_array = fsim.ppsfp({tv});
    return det_list_array[0];
  }
  else {
    return fsim.sppfp(tv);
  }
}

// SPPFP/ppsfp のテスト
std::pair<int, int>
test1(
  const TpgNetwork& network,
  Fsim& fsim,
  const std::vector<TestVector>& tv_list,
  SizeType batch_size,
  bool ppsfp
)
{
  int det_num = 0;
  int nepat = 0;
  std::vector<bool> det_array(network.max_fault_id(), false);

  auto ntv = tv_list.size();
  if ( batch_size == 1 ) {
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto tv = tv_list[i];
      auto det_list = simulate1(fsim, tv, ppsfp);
      bool detected = false;
      for ( auto fault: det_list ) {
	auto fid = fault.id();
	if ( !det_array[fid] ) {
	  det_array[fid] = true;
	  ++ det_num;
	  print_fault(fault, i);
	  detected = true;
	}
      }
      if ( detected ) {
	++ nepat;
      }
    }
  }
  else if ( batch_size == ntv ) {
    auto det_list_array = ppsfp ? fsim.ppsfp(tv_list) : fsim.sppfp(tv_list);
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto& det_list = det_list_array[i];
      bool detected = false;
      for ( auto fault: det_list ) {
	auto fid = fault.id();
	if ( !det_array[fid] ) {
	  det_array[fid] = true;
	  ++ det_num;
	  print_fault(fault, i);
	  detected = true;
	}
      }
      if ( detected ) {
	++ nepat;
      }
    }
  }
  else {
    for ( SizeType base = 0; base < ntv; base += batch_size ) {
      auto n = std::min(batch_size, ntv - base);
      std::vector<TestVector> tv_buff(n);
      for ( SizeType i = 0; i < n; ++ i ) {
	tv_buff[i] = tv_list[base + i];
      }
      auto det_list_array = ppsfp ? fsim.ppsfp(tv_buff) : fsim.sppfp(tv_buff);
      for ( SizeType i = 0; i < n; ++ i ) {
	auto& det_list = det_list_array[i];
	bool detected = false;
	for ( auto fault: det_list ) {
	  auto fid = fault.id();
	  if ( !det_array[fid] ) {
	    det_array[fid] = true;
	    ++ det_num;
	    print_fault(fault, base + i);
	    detected = true;
	  }
	}
	if ( detected ) {
	  ++ nepat;
	}
      }
    }
  }

  return std::make_pair(det_num, nepat);
}

// SPPFP2/PPSFP2 のテスト
std::pair<int, int>
test2(
  const TpgNetwork& network,
  Fsim& fsim,
  const std::vector<TestVector>& tv_list,
  SizeType batch_size,
  bool ppsfp
)
{
  int det_num = 0;
  int nepat = 0;
  std::vector<bool> det_array(network.max_fault_id(), false);

  auto ntv = tv_list.size();
  if ( batch_size == 1 ) {
    for ( SizeType i = 0; i < ntv; ++ i ) {
      auto tv = tv_list[i];
      auto res = ppsfp ? fsim.ppsfp2({tv}) : fsim.sppfp2(tv);
      bool detected = false;
      auto n = res.det_num(0);
      for ( SizeType i = 0; i < n; ++ i ) {
	auto fault = res.fault(0, i);
	auto fid = fault.id();
	if ( !det_array[fid] ) {
	  det_array[fid] = true;
	  ++ det_num;
	  print_fault(fault, i);
	  detected = true;
	}
      }
      if ( detected ) {
	++ nepat;
      }
    }
  }
  else if ( batch_size == ntv ) {
    auto res = ppsfp ? fsim.ppsfp2(tv_list) : fsim.sppfp2(tv_list);
    for ( SizeType tv_id = 0; tv_id < ntv; ++ tv_id ) {
      bool detected = false;
      auto n = res.det_num(tv_id);
      for ( SizeType i = 0; i < n; ++ i ) {
	auto fault = res.fault(tv_id, i);
	auto fid = fault.id();
	if ( !det_array[fid] ) {
	  det_array[fid] = true;
	  ++ det_num;
	  print_fault(fault, tv_id);
	  detected = true;
	}
      }
      if ( detected ) {
	++ nepat;
      }
    }
  }
  else {
    for ( SizeType base = 0; base < ntv; base += batch_size ) {
      auto n = std::min(batch_size, ntv - base);
      std::vector<TestVector> tv_buff(n);
      for ( SizeType i = 0; i < n; ++ i ) {
	tv_buff[i] = tv_list[base + i];
      }
      auto res = ppsfp ? fsim.ppsfp2(tv_buff) : fsim.sppfp2(tv_buff);
      for ( SizeType i = 0; i < n; ++ i ) {
	auto det_num = res.det_num(i);
	bool detected = false;
	for ( SizeType j = 0; j < det_num; ++ j ) {
	  auto fault = res.fault(i, j);
	  auto fid = fault.id();
	  if ( !det_array[fid] ) {
	    det_array[fid] = true;
	    ++ det_num;
	    print_fault(fault, base + i);
	    detected = true;
	  }
	}
	if ( detected ) {
	  ++ nepat;
	}
      }
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
  bool ppsfp2 = false;
  bool sppfp = false;
  bool sppfp2 = false;

  bool sa_mode = false;
  bool td_mode = false;

  bool drop = false;

  bool multi = false;
  int thread_num = 0;
  int batch_size = 0;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    std::string arg = argv[pos];
    if ( arg[0] == '-' ) {
      if ( arg == "-n" || arg == "--num" ) {
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
      if ( arg == "-b" || arg == "--batch-size" ) {
	++ pos;
	if ( pos >= argc ) {
	  std::cerr << " -b option requires #pat"
		    << std::endl;
	  return -1;
	}
	batch_size = atoi(argv[pos]);
	if ( batch_size == 0 ) {
	  std::cerr << " integer expected after -b"
		    << std::endl;
	  return -1;
	}
      }
      else if ( arg == "--fsim2" ) {
	if ( fsim3 ) {
	  std::cerr << "--fsim2 and --fsim3 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	fsim2 = true;
      }
      else if ( arg == "--fsim3" ) {
	if ( fsim2 ) {
	  std::cerr << "--fsim2 and --fsim3 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	fsim3 = true;
      }
      else if ( arg == "--ppsfp" ) {
	if ( sppfp ) {
	  std::cerr << "--ppspf and --sppfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( sppfp2 ) {
	  std::cerr << "--ppsfp and --sppfp2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp2 ) {
	  std::cerr << "--ppspf and --ppsfp2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	ppsfp = true;
      }
      else if ( arg == "--ppsfp2" ) {
	if ( sppfp ) {
	  std::cerr << "--ppspf2 and --sppfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( sppfp2 ) {
	  std::cerr << "--ppspf2 and --sppfp2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp ) {
	  std::cerr << "--ppspf2 and --ppsfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	ppsfp2 = true;
      }
      else if ( arg == "--sppfp" ) {
	if ( sppfp2 ) {
	  std::cerr << "--sppfp and --sppfp2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp ) {
	  std::cerr << "--sppfp and --ppspf are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp2 ) {
	  std::cerr << "--sppfp and --ppspf2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sppfp = true;
      }
      else if ( arg == "--sppfp2" ) {
	if ( sppfp ) {
	  std::cerr << "--sppfp2 and --sppfp are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp ) {
	  std::cerr << "--sppfp2 and --ppspf are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	if ( ppsfp2 ) {
	  std::cerr << "--sppfp2 and --ppspf2 are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sppfp2 = true;
      }
      else if ( arg == "--stuck-at" ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( arg == "--transition-delay" ) {
	if ( td_mode ) {
	  std::cerr << "--stuck-at and --transition-delay are mutually exclusive"
		    << std::endl;
	  return -1;
	}
	td_mode = true;
      }
      else if ( arg == "--drop" ) {
	drop = true;
      }
      else if ( arg == "--multi" ) {
	multi = true;
      }
      else if ( arg == "--thread-num" ) {
	++ pos;
	if ( pos >= argc ) {
	  std::cerr << " --thread-num option requires <int>"
		    << std::endl;
	  return -1;
	}
	thread_num = atoi(argv[pos]);
      }
      else if ( arg == "--blif" ) {
	format = "blif";
      }
      else if ( arg == "--iscas89" ) {
	format = "iscas89";
      }
      else if ( arg == "--verbose" ) {
	verbose = true;
      }
      else {
	std::cerr << arg << ": illegal option"
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

  if ( batch_size == 0 ) {
    batch_size = npat;
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
  option.add("thread_num", thread_num);
  auto fsim = Fsim(fault_list, option);

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
  if ( ppsfp || sppfp ) {
    dpnum = test1(network, fsim, tv_list, batch_size, ppsfp);
  }
  else if ( ppsfp2 || sppfp2 ) {
    dpnum = test2(network, fsim, tv_list, batch_size, ppsfp2);
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
