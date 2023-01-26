
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgTest.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "Fsim.h"
#include "DopVerifyResult.h"


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
      else if ( strcmp(argv[pos], "--ffr-new") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--ffr-new  and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "ffr_new";
      }
      else if ( strcmp(argv[pos], "--mffc-new") == 0 ) {
	if ( mode != string{} ) {
	  cerr << "--mffc_new and --" << mode << " are mutually exclusive" << endl;
	  return -1;
	}
	mode = "mffc_new";
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
  auto test = DtpgTest::new_test(mode, network, fault_type, just_type, solver_type);

  auto count = test->do_test(verbose);

  const DopVerifyResult& verify_result = test->verify_result();
  int n = verify_result.error_count();

  return n;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::dtpg_test(argc, argv);
}
