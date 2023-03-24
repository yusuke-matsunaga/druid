
/// @file TpgFaultMgrTest.cc
/// @brief TpgFaultMgrTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFaultMgr.h"
#include <libgen.h>


BEGIN_NAMESPACE_DRUID

void
faultmgr_test(
  const string& filename,
  FaultType fault_type
)
{
  auto network = TpgNetwork::read_blif(filename);
  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(network, fault_type);

  for ( auto f: fmgr.rep_fault_list() ) {
    cout << f << endl;
  }
}

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " filename" << endl;
}

END_NAMESPACE_DRUID

int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using namespace DRUID_NAMESPACE;

  argv0 = basename(argv[0]);

  FaultType fault_type = FaultType::StuckAt;

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--stuck_at") == 0 ) {
	fault_type = FaultType::StuckAt;
      }
      else if ( strcmp(argv[pos], "--transition_delay") == 0 ) {
	fault_type = FaultType::TransitionDelay;
      }
      else if ( strcmp(argv[pos], "--gate_exhaustive") == 0 ) {
	fault_type = FaultType::GateExhaustive;
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

  faultmgr_test(argv[pos], fault_type);

  return 0;
}
