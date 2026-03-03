
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


BEGIN_NAMESPACE_DRUID

int
minpat(
  int argc,
  char** argv
)
{
  if ( argc != 2 ) {
    std::cout << "Usage: minpat <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];

  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto dtpg_option = JsonValue::object();
  dtpg_option.add("group_mode", JsonValue("ffr_mt"));
  auto fault_list = network.rep_fault_list();

  MinPat minpat(network);
  auto tv_list = minpat.run(fault_list);

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
