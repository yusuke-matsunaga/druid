
/// @file dichotomy_test.cc
/// @brief dichotomy のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Dichotomy.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "types/TestVector.h"
#include "dtpg/DtpgMgr.h"
#include "fsim/Fsim.h"
#include "ym/JsonValue.h"
#include <unistd.h> // getopt
#include <libgen.h> // basename


BEGIN_NAMESPACE_DRUID

extern
TpgFaultList
ffr_reduction(
  const TpgFaultList& fault_list,
  const ConfigParam& option
);

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " [--blif|--iscas89] <file>"
	    << std::endl;
}

int
dichotomy_test(
  int argc,
  char** argv
)
{
  std::string format = "blif";
  FaultType ftype = FaultType::StuckAt;
  bool do_ffr_reduction = false;
  bool multi_thread = false;
  bool verbose = false;
  int debug = 0;
  bool dump = false;

  SizeType argpos = 1;
  for ( ; argpos < argc; ++ argpos ) {
    if ( argv[argpos][0] != '-' ) {
      break;
    }
    std::string arg = argv[argpos];
    if ( arg == "--blif" ) {
      format = "blif";
    }
    else if ( arg == "--iscas89" ) {
      format = "iscas89";
    }
    else if ( arg == "--stuck-at" ) {
      ftype = FaultType::StuckAt;
    }
    else if ( arg == "--transition-delay" ) {
      ftype = FaultType::TransitionDelay;
    }
    else if ( arg == "--ffr-reduction" ) {
      do_ffr_reduction = true;
    }
    else if ( arg == "--multi-thread" ) {
      multi_thread = true;
    }
    else if ( arg == "--verbose" ) {
      verbose = true;
    }
    else if ( arg == "--debug" ) {
      ++ debug;
    }
    else if ( arg == "--dump" ) {
      dump = true;
    }
    else {
      std::cerr << arg << ": Unknown option" << std::endl;
      usage();
      return 1;
    }
  }
  if ( argpos != argc - 1 ) {
    usage();
    std::cout << "Usage: minpat <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[argpos];

  TpgNetwork network;
  try {
    network = TpgNetwork::read_network(filename, format, ftype);
  }
  catch ( std::invalid_argument error ) {
    std::cerr << error.what() << std::endl;
    return 1;
  }

  if ( dump ) {
    network.print(std::cout);
  }

  auto global_option = JsonValue::object();
  global_option.add("multi_thread", multi_thread);
  global_option.add("verbose", verbose);
  global_option.add("debug", debug);

  auto option = JsonValue::object();
  option.add("*", global_option);

  auto fault_list = network.rep_fault_list();

  std::mt19937 randgen;

  auto res = DtpgMgr::run(fault_list, option);
  TpgFaultList det_fault_list;
  std::vector<TestVector> tv_list;
  for ( auto fault: fault_list ) {
    if ( res.status(fault) == FaultStatus::Detected ) {
      det_fault_list.push_back(fault);
      auto tv = res.testvector(fault);
      tv.fix_x_from_random(randgen);
      tv_list.push_back(tv);
    }
  }
  std::swap(fault_list, det_fault_list);

  if ( do_ffr_reduction ) {
    fault_list = ffr_reduction(fault_list, option);
  }

  auto group_mgr = Dichotomy::run(fault_list, option);
  auto& group_list = group_mgr.group_list();

  std::vector<std::pair<SizeType, SizeType>> conflict_pair_list;
  { // conflict_list が対称的かチェックする．
    std::unordered_set<SizeType> mark;
    auto ng = group_list.size();
    for ( SizeType id = 0; id < ng; ++ id ) {
      auto group = group_list[id];
      for ( auto group1: group->conflict_list() ) {
	auto id1 = group1->id();
	mark.insert(id * ng + id1);
	if ( id < id1 ) {
	  conflict_pair_list.push_back({id, id1});
	}
      }
    }
    for ( SizeType id = 0; id < ng; ++ id ) {
      auto group = group_list[id];
      for ( auto group1: group->conflict_list() ) {
	auto id1 = group1->id();
	if ( mark.count(id1 * ng + id) == 0 ) {
	  std::cout << "Error!: conflict relation should be symmetry";
	  abort();
	}
      }
    }
  }

  std::cout << "# of faults: " << fault_list.size() << std::endl;
  std::cout << "# of Groups: " << group_list.size() << std::endl;
  for ( auto group: group_list ) {
    if ( group->fault_list().size() == 1 ) {
      continue;
    }
    std::cout << "Group#" << group->id();
    for ( auto fault: group->fault_list() ) {
      std::cout << " " << fault.str();
    }
#if 0
    std::cout << std::endl
	      << "  conflict: ";
    for ( auto group1: group->conflict_list() ) {
      for ( auto fault: group1->fault_list() ) {
	std::cout << " " << fault.str();
      }
    }
#endif
#if 0
    std::cout << std::endl
	      << "  dominate: ";
    for ( auto group1: group->dominate_list() ) {
      for ( auto fault: group1->fault_list() ) {
	std::cout << " " << fault.str();
      }
    }
#endif
    std::cout << std::endl
	      << std::endl;
  }

  if ( group_mgr.undet_group() != nullptr ) {
    std::cout << "# of Undet Groups: "
	      << group_mgr.undet_group()->fault_list().size()
	      << std::endl;
#if 0
    for ( auto fault: group_mgr.undet_group()->fault_list() ) {
      std::cout << " " << fault.str();
    }
    std::cout << std::endl;
#endif
  }
  std::cout << "# of Conflict pairs: "
	    << conflict_pair_list.size()
	    << std::endl;
#if 0
  for ( auto& p: conflict_pair_list ) {
    auto id1 = p.first;
    auto id2 = p.second;
    std::cout << "Group#" << id1 << ", Group#" << id2 << std::endl;
  }
#endif

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::dichotomy_test(argc, argv);
}
