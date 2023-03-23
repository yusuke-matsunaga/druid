
/// @file TpgNetworkTest.cc
/// @brief TpgNetworkTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_DRUID

void
loop_test(
  ymuint loop_count,
  const string& filename
)
{
  for (ymuint i1 = 0; i1 < loop_count; ++ i1) {
    for (ymuint i2 = 0; i2 < loop_count; ++ i2) {
      cout << "            "
	   << "\r" << i1 << ": " << i2 << "\r";
      cout.flush();
      try {
	auto network = TpgNetwork::read_iscas89(filename);
	TpgFaultMgr fmgr;
	fmgr.gen_fault_list(network, FaultType::StuckAt);
      }
      catch ( std::invalid_argument ) {
	cerr << "Error in reading " << filename << endl;
	break;
      }
    }
  }
  cout << endl;
}

END_NAMESPACE_DRUID

int
main(int argc,
     char** argv)
{
  using namespace std;

  if ( argc != 2 ) {
    cerr << "USAGE: " << argv[0] << " filename" << endl;
    return 255;
  }

  DRUID_NAMESPACE::loop_test(100, argv[1]);

  return 0;
}
