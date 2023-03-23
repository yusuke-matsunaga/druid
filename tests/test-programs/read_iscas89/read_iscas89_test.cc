
/// @file read_iscas89_test.cc
/// @brief read_iscas89_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

int
read_iscas89_test(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    return 10;
  }
  auto network = TpgNetwork::read_iscas89(argv[1]);
  network.print(cout);

  return 0;
}

END_NAMESPACE_DRUID


int
main(
  int argc,
  char** argv
)
{
  return nsYm::nsDruid::read_iscas89_test(argc, argv);
}
