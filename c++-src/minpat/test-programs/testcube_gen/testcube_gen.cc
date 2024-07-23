
/// @file testcube_gen.cc
/// @brief TestCoverGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "Fsim.h"
#include "FaultInfoMgr.h"
#include "Reducer.h"
#include "TestCoverGen.h"
#include "FaultGroupGen.h"
#include "ColGraph.h"
#include "Dsatur.h"
#include "Isx.h"
#include "ColGraph_cube.h"
#include "Dsatur_cube.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

int
testcube_gen(
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
  bool multi = false;
  bool verbose = false;
  string just_type;
  int loop = 1;
  int cube_per_fault = 0;
  bool dsatur = false;
  bool dsatur2 = false;
  bool isx = false;
  int isx_limit = 0;
  bool isx_skip = false;
  bool debug_dtpg = false;
  bool debug_fault_reduce = false;
  bool debug_testcube_gen = false;
  bool debug_colgraph = false;
  bool debug_dsatur = false;
  bool debug_isx = false;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--sat_type") == 0 ) {
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
      else if ( strcmp(argv[pos], "--multi") == 0 ) {
	multi = true;
      }
      else if ( strcmp(argv[pos], "--loop") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  loop = atoi(argv[pos]);
	}
	else {
	  cerr << "--loop requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--cube-per-fault") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  cube_per_fault = atoi(argv[pos]);
	}
	else {
	  cerr << "--cube-per-fault requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--dsatur") == 0 ) {
	dsatur = true;
      }
      else if ( strcmp(argv[pos], "--dsatur2") == 0 ) {
	dsatur2 = true;
      }
      else if ( strcmp(argv[pos], "--isx") == 0 ) {
	isx = true;
	++ pos;
	if ( pos < argc ) {
	  isx_limit = atoi(argv[pos]);
	}
	else {
	  cerr << "--isx requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--isx2") == 0 ) {
	isx = true;
	isx_skip = true;
	++ pos;
	if ( pos < argc ) {
	  isx_limit = atoi(argv[pos]);
	}
	else {
	  cerr << "--isx requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--debug-all") == 0 ) {
	debug_dtpg = true;
	debug_fault_reduce = true;
	debug_testcube_gen = true;
	debug_colgraph = true;
	debug_dsatur = true;
	debug_isx = true;
      }
      else if ( strcmp(argv[pos], "--debug-dtpg") == 0 ) {
	debug_dtpg = true;
      }
      else if ( strcmp(argv[pos], "--debug-fault_reduce") == 0 ) {
	debug_fault_reduce = true;
      }
      else if ( strcmp(argv[pos], "--debug-testcube_gen") == 0 ) {
	debug_testcube_gen = true;
      }
      else if ( strcmp(argv[pos], "--debug-colgraph") == 0 ) {
	debug_colgraph = true;
      }
      else if ( strcmp(argv[pos], "--debug-dsatur") == 0 ) {
	debug_dsatur = true;
      }
      else if ( strcmp(argv[pos], "--debug-isx") == 0 ) {
	debug_isx = true;
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

  if ( !sa_mode && !td_mode ) {
    // sa_mode をデフォルトにする．
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  string filename = argv[pos];
  auto network = TpgNetwork::read_network(filename, format, fault_type);

  unordered_map<string, JsonValue> option_dict;
  if ( just_type != "" ) {
    option_dict.emplace("justifier", just_type);
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    option_dict.emplace("sat_param", sat_obj);
  }
  option_dict.emplace("debug", JsonValue{debug_dtpg});
  JsonValue option{option_dict};

  auto fault_list = network.rep_fault_list();

  Timer total_timer;
  total_timer.start();

  Timer timer;
  timer.start();

  FaultInfoMgr finfo_mgr{network, fault_list};

  finfo_mgr.generate(option);

  vector<const TpgFault*> det_fault_list;
  det_fault_list.reserve(fault_list.size());
  for ( auto f: fault_list ) {
    auto& finfo = finfo_mgr.fault_info(f);
    if ( finfo.status() == FaultStatus::Detected ) {
      det_fault_list.push_back(f);
    }
  }

  unordered_map<string, JsonValue> fr_option_dict;
  if ( debug_fault_reduce ) {
    fr_option_dict.emplace("debug", JsonValue{true});
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    fr_option_dict.emplace("sat_param", sat_obj);
  }
  if ( cube_per_fault > 0 ) {
    fr_option_dict.emplace("cube_per_fault", JsonValue{cube_per_fault});
  }
  fr_option_dict.emplace("loop_limit", JsonValue{loop});
  JsonValue fr_option{fr_option_dict};

  Reducer::reduce(finfo_mgr, fr_option);
  auto cover_list = TestCoverGen::run(finfo_mgr, fr_option);

  timer.stop();

  SizeType cube_num = 0;
  for ( auto& cover: cover_list ) {
    cube_num += cover.cube_list().size();
  }

  cout << "=========================================" << endl
       << "Detected Faults:     " << det_fault_list.size() << endl
       << "Reduced Faults:      " << cover_list.size() << endl
       << "Total # of cubes:    " << cube_num << endl
       << "CPU time:            " << timer.get_time() << endl;

  timer.reset();
  timer.start();

  unordered_map<string, JsonValue> cg_option_dict;
  if ( debug_colgraph ) {
    cg_option_dict.emplace("debug", JsonValue{true});
  }
  JsonValue cg_option{cg_option_dict};

  unordered_map<string, JsonValue> ds_option_dict;
  if ( debug_dsatur ) {
    ds_option_dict.emplace("debug", JsonValue{true});
  }
  JsonValue ds_option{ds_option_dict};

  unordered_map<string, JsonValue> isx_option_dict;
  if ( debug_isx ) {
    isx_option_dict.emplace("debug", JsonValue{true});
  }
  if ( isx_skip ) {
    isx_option_dict.emplace("skip", JsonValue{true});
  }
  JsonValue isx_option{isx_option_dict};

  vector<TestVector> tv_list;
  if ( dsatur ) {
    ColGraph cg{network, cover_list, cg_option};
    Dsatur ds{cg, ds_option};
    ds.coloring();
    SizeType nc = cg.color_num();
    for ( SizeType col = 1; col <= nc; ++ col ) {
      auto tv = cg.testvector(col);
      tv_list.push_back(tv);
    }
  }
  else if ( dsatur2 ) {
    ColGraph_cube cg{network, cover_list, cg_option};
    Dsatur_cube ds{cg};
    ds.coloring();
    SizeType nc = cg.color_num();
    for ( SizeType col = 1; col <= nc; ++ col ) {
      auto tv = cg.testvector(col);
      tv_list.push_back(tv);
    }
  }
  else if ( isx ) {
    ColGraph cg{network, cover_list, cg_option};
    Isx isx{cg, isx_option};
    isx.coloring(isx_limit);
    Dsatur ds{cg, ds_option};
    ds.coloring();
    SizeType nc = cg.color_num();
    for ( SizeType col = 1; col <= nc; ++ col ) {
      auto tv = cg.testvector(col);
      tv_list.push_back(tv);
    }
  }
  else {
    FaultGroupGen fgg{network, fr_option};
    tv_list = fgg.generate(cover_list);
  }

  timer.stop();
  total_timer.stop();

  cout << "=========================================" << endl
       << "# of patterns        " << tv_list.size() << endl
       << "CPU time:            " << timer.get_time() << endl
       << "=========================================" << endl
       << "Total CPU time:      " << total_timer.get_time() << endl;

  { // verify
    Fsim fsim{network, det_fault_list, false, false};
    vector<TestVector> tv_buff;
    tv_buff.reserve(PV_BITLEN);
    SizeType n = 0;
    vector<bool> mark(network.max_fault_id(), false);
    for ( auto& tv: tv_list ) {
      tv_buff.push_back(tv);
      if ( tv_buff.size() == PV_BITLEN ) {
	fsim.ppsfp(tv_buff,
		   [&](const TpgFault* f,
		       const DiffBitsArray& _) {
		     fsim.set_skip(f);
		     mark[f->id()] = true;
		     ++ n;
		   });
	tv_buff.clear();
      }
    }
    if ( !tv_buff.empty() ) {
      fsim.ppsfp(tv_buff,
		 [&](const TpgFault* f,
		     const DiffBitsArray& _) {
		   fsim.set_skip(f);
		   mark[f->id()] = true;
		   ++ n;
		 });
    }
    if ( n != det_fault_list.size() ) {
      cout << "Error!" << endl
	   << "# of detected faults: " << n << endl;
      for ( auto& cover: cover_list ) {
	auto fault = cover.fault();
	if ( !mark[fault->id()] ) {
	  cout << fault->str() << " is not covered" << endl;
	}
      }
    }
  }

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::testcube_gen(argc, argv);
}
