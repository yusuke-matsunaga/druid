
/// @file read_blif_test.cc
/// @brief read_blif_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgNetwork.h"
#include "types/FaultType.h"


BEGIN_NAMESPACE_DRUID

int
read_blif_test(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    return 10;
  }
  auto network = TpgNetwork::read_blif(argv[1], FaultType::StuckAt);
  network.print(std::cout);

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return nsYm::nsDruid::read_blif_test(argc, argv);
}
