
/// @file count_test.cc
/// @brief CnfGen を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "FaultType.h"
#include "Fsim.h"
#include "CondGenMgr.h"
#include "CnfGen.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "ym/CnfSize.h"
#include "ym/Timer.h"
#include <random>


BEGIN_NAMESPACE_DRUID

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <file>" << endl;
}

int
count_test(
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
  int limit = 100;
  bool ffr_mode = false;
  bool do_finfo_mgr = false;
  bool do_reduction = true;
  bool do_ffr_reduction = false;
  bool do_global_reduction = false;
  bool bdd = false;
  bool cube = false;
  int debug_level = 0;

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
      else if ( strcmp(argv[pos], "--fault-info-mgr") == 0 ) {
	do_finfo_mgr = true;
      }
      else if ( strcmp(argv[pos], "--no-reduction") == 0 ) {
	do_reduction = false;
      }
      else if ( strcmp(argv[pos], "--ffr_reduction") == 0 ) {
	do_ffr_reduction = true;
      }
      else if ( strcmp(argv[pos], "--global_reduction") == 0 ) {
	do_global_reduction = true;
      }
      else if ( strcmp(argv[pos], "--limit") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  limit = atoi(argv[pos]);
	}
	else {
	  cerr << "--limit requires <int> argument" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--ffr") == 0 ) {
	ffr_mode = true;
      }
      else if ( strcmp(argv[pos], "--bdd") == 0 ) {
	bdd = true;
      }
      else if ( strcmp(argv[pos], "--cube") == 0 ) {
	cube = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else if ( strcmp(argv[pos], "--debug") == 0 ) {
	++ pos;
	if ( pos < argc ) {
	  debug_level = atoi(argv[pos]);
	}
	else {
	  cerr << "--debug requires <int> argument" << endl;
	  return -1;
	}
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

  if ( verbose ) {
    cout << "Total " << network.rep_fault_list().size() << " faults" << endl;
  }

  unordered_map<string, JsonValue> option_dict;
  if ( just_type != "" ) {
    option_dict.emplace("justifier", just_type);
  }
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    option_dict.emplace("sat_param", sat_obj);
  }
  JsonValue option{option_dict};

  unordered_map<string, JsonValue> cg_option_dict;
  if ( sat_type != string{} ) {
    auto sat_obj = JsonValue{sat_type};
    cg_option_dict.emplace("sat_param", sat_obj);
  }
  cg_option_dict.emplace("debug", JsonValue{debug_level});
  JsonValue cg_option{cg_option_dict};

  auto src_fault_list = network.rep_fault_list();

  Timer total_timer;
  total_timer.start();

  StructEngine engine{network}; // ダミー

  unordered_map<string, JsonValue> cnf_option_dict;
  if ( bdd ) {
    cnf_option_dict.emplace("method", "bdd");
  }
  else if ( cube ) {
    cnf_option_dict.emplace("method", "cube");
  }
  JsonValue cnf_option{cnf_option_dict};

  auto total_cnf_size = CnfSize::zero();
  auto total_cnf_size_naive = CnfSize::zero();
  auto total_cnf_size_real = CnfSize::zero();
  if ( ffr_mode ) {
    total_cnf_size = CondGenMgr::calc_root_cond_size(network, limit,
						     cg_option,
						     cnf_option);
    total_cnf_size_naive = CondGenMgr::calc_root_cond_size(network, limit,
							   cg_option,
							   JsonValue{});

    for ( auto ffr: network.ffr_list() ) {
      auto root = ffr->root();
      StructEngine engine0{network};
      engine0.make_cnf({root}, {root});
      auto size0 = engine0.solver().cnf_size();
      StructEngine engine1{network};
      auto bd_enc = new BoolDiffEnc{engine1, root};
      engine1.make_cnf({}, {root});
      auto size1 = engine1.solver().cnf_size();
      auto size = size1 - size0;
      total_cnf_size_real += size;
    }
  }
  else {
    auto cond_array = CondGenMgr::fault_cond(network, src_fault_list, limit,
					    cg_option);
    for ( auto fault: src_fault_list ) {
      auto cond = cond_array[fault->id()];
      auto cnf_size = CnfGen::calc_cnf_size(cond, cnf_option);
      cout << fault->str()
	   << ": " << endl
	   << "| " << cnf_size << endl;
      StructEngine engine{network};
      auto root = fault->ffr_root();
      engine.make_cnf({root}, {root});
      auto cnf_size0 = engine.solver().cnf_size();
      CnfGen::make_cnf(engine, cond);
      auto cnf_size1 = engine.solver().cnf_size();
      auto real_size = cnf_size1 - cnf_size0;
      cout << "real_size: " << real_size << endl;
      cout << "mandatory cond: " << cond.mandatory_condition() << endl;
      cout << "cover" << endl;
      for ( auto& cube: cond.cube_list() ) {
	cout << cube << endl;
      }
      total_cnf_size += cnf_size;
    }
  }

  cout << "Total CNF size:         "
       << setw(10) << total_cnf_size.clause_num
       << " "
       << setw(10) << total_cnf_size.literal_num << endl;
  cout << "Total CNF size(naive):  "
       << setw(10) << total_cnf_size_naive.clause_num
       << " "
       << setw(10) << total_cnf_size_naive.literal_num << endl;
  cout << "Total CNF size(real):   "
       << setw(10) << total_cnf_size_real.clause_num
       << " "
       << setw(10) << total_cnf_size_real.literal_num << endl;

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return DRUID_NAMESPACE::count_test(argc, argv);
}
