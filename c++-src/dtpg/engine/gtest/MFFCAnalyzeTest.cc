
/// @file MFFCAnalyzeTest.cc
/// @brief MFFCAnalyze のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "dtpg/BdEngine.h"
#include "dtpg/NaiveDtpgEngine.h"
#include "dtpg/NaiveDualEngine.h"
#include "BoolDiffEnc.h"
#include "types/TpgNetwork.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

TEST(MFFCAnalyzeTest, test1)
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto filename = data_dir / "s27.blif";
  auto network = TpgNetwork::read_blif(filename, FaultType::StuckAt);
  auto fault_list = network.rep_fault_list();
  TpgFault fault1;
  TpgFault fault2;
  for ( auto fault: fault_list ) {
    if ( fault.str() == "G#9:I1:SA0" ) {
      fault1 = fault;
    }
    else if ( fault.str() == "G#8:O:SA0" ) {
      fault2 = fault;
    }
  }
  auto ffr1 = network.ffr(fault1);
  auto ffr2 = network.ffr(fault2);
  auto mffc = network.mffc(fault1);

  BdEngine engine(mffc.root());
  auto pvar = engine.prop_var();
  engine.solver().add_clause(pvar);
  auto enc1 = new BoolDiffEnc(ffr1.root(), mffc.root());
  auto enc2 = new BoolDiffEnc(ffr2.root(), mffc.root());
  engine.add_subenc(std::unique_ptr<SubEnc>{enc1});
  engine.add_subenc(std::unique_ptr<SubEnc>{enc2});

  auto pvar1 = enc1->prop_var();
  auto cond1 = fault1.ffr_propagate_condition();
  auto lits1 = engine.conv_to_literal_list(cond1);
  lits1.push_back(pvar1);

  auto pvar2 = enc2->prop_var();
  auto cond2 = fault2.ffr_propagate_condition();
  auto lits2 = engine.conv_to_literal_list(cond2);
  lits2.push_back(pvar1);

  {
    auto res = engine.solve(lits1);
    EXPECT_EQ( SatBool3::True, res );
  }
  {
    auto res = engine.solve(lits2);
    EXPECT_EQ( SatBool3::True, res );
  }
  {
    auto tmp_lits = concat_lits(lits1, lits2);
    auto res = engine.solve(tmp_lits);
    EXPECT_EQ( SatBool3::True, res );
  }
}

END_NAMESPACE_DRUID
