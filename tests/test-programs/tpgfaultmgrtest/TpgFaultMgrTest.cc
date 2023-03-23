
/// @file TpgFaultMgrTest.cc
/// @brief TpgFaultMgrTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_DRUID

void
faultmgr_test(
  const string& filename
)
{
  auto network = TpgNetwork::read_blif(filename);
  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(network, FaultType::StuckAt);

  for ( auto f: fmgr.rep_fault_list() ) {
    cout << f << endl;
  }
}

END_NAMESPACE_DRUID

int
main(
  int argc,
  char** argv
)
{
  using namespace std;

  if ( argc != 2 ) {
    cerr << "USAGE: " << argv[0] << " filename" << endl;
    return 255;
  }

  DRUID_NAMESPACE::faultmgr_test(argv[1]);

  return 0;
}
