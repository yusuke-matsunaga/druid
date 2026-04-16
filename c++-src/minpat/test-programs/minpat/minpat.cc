
/// @file minpat.cc
/// @brief minpat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "MinPat.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "types/TestVector.h"
#include "fsim/Fsim.h"
#include "ym/JsonValue.h"
#include <unistd.h> // getopt
#include <libgen.h> // basename


BEGIN_NAMESPACE_DRUID

static char* argv0;

void
usage()
{
  std::cerr << "USAGE: " << basename(argv0)
	    << " [--blif|--iscas89] [--ffr-reduction] <file>"
	    << std::endl;
}

int
minpat(
  int argc,
  char** argv
)
{
  std::string format = "blif";
  FaultType ftype = FaultType::StuckAt;
  bool random_fix = false;
  bool ffr_reduction = false;
  std::string comp_type = "simple";
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
    else if ( arg == "--random-fix" ) {
      random_fix = true;
    }
    else if ( arg == "--ffr-reduction" ) {
      ffr_reduction = true;
    }
    else if ( arg == "--merge" ) {
      comp_type = "merge";
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

  auto fault_list = network.rep_fault_list();

  auto dtpg_option = JsonValue::object();
  dtpg_option.add("group_mode", JsonValue("ffr"));
  dtpg_option.add("extractor", JsonValue("simple"));

  auto init_option = JsonValue::object();
  init_option.add("dtpg", dtpg_option);
  init_option.add("random_fix", random_fix);

  auto reduce_option = JsonValue::object();
  reduce_option.add("ffr", ffr_reduction);

  auto comp_option = JsonValue::object();
  comp_option.add("type", comp_type);

  auto global_option = JsonValue::object();
  global_option.add("multi_thread", multi_thread);
  global_option.add("verbose", verbose);
  global_option.add("debug", debug);

  auto option = JsonValue::object();
  option.add("init", init_option);
  option.add("reduce", reduce_option);
  option.add("comp", comp_option);
  option.add("*", global_option);

  auto tv_list = MinPat::run(network, fault_list, option);

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::minpat(argc, argv);
}
