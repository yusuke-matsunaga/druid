
/// @file TpgFaultTest.cc
/// @brief TpgFaultImpl のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "TpgFaultImpl.h"
#include "ym/BnModel.h"


BEGIN_NAMESPACE_DRUID

TEST(TpgFaultTest, buf_sa)
{
  BnModel model;
  auto a = model.new_input();
  auto node = model.new_primitive(PrimType::Buf, {a});
  model.new_output(node);

  auto tpg_network = TpgNetwork{model};

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(tpg_network, FaultType::StuckAt);


}

END_NAMESPACE_DRUID
