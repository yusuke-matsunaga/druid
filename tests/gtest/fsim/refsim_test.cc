
/// @file refsim_test.cc
/// @brief refsim_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "RefSim.h"
#include "TpgNetwork.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

int
refsim_test(
  int argc,
  char** argv
)
{
  TpgNetwork network;
  if ( argc == 1 ) {
    auto filename = string{TESTDATA_DIR} + "/" + "s27.blif";
    network = TpgNetwork::read_blif(filename);
  }
  else {
    auto filename = argv[1];
    network = TpgNetwork::read_blif(filename);
  }

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(network, FaultType::StuckAt);

  auto fault_list = fmgr.fault_list();

  SizeType input_num = network.input_num();
  SizeType dff_num = network.dff_num();

  RefSim refsim{network};

  std::mt19937 randgen;
  SizeType nv = 1000;

  TestVector tv(input_num, dff_num, false);
  for ( SizeType i = 0; i < nv; ++ i ) {
    tv.set_from_random(randgen);
    for ( auto fault: fault_list ) {
      auto ref_dbits = refsim.simulate_sa(tv, fault);
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
  return DRUID_NAMESPACE::refsim_test(argc, argv);
}
