
/// @file cnfgen_test.cc
/// @brief cnfgen_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "FaultType.h"
#include "CondGen.h"
#include "CnfGenMgr.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

string mydata[] = {
  "s27.blif",
  "s1196.blif",
  "s5378.blif",
  "s9234.blif"
};

string mymethod[] = {
  "naive",
  "cover",
  "factor",
  "aig"
};


class CondGenTestWithParam :
  public ::testing::TestWithParam<std::tuple<string, string>>
{
public:

  /// @brief コンストラクタ
  CondGenTestWithParam();

  /// @brief テストを行う．
  void
  do_test();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  SetUp();

  /// @brief 終了処理を行う．
  void
  TearDown();

  /// @brief テストパラメータからファイル名を取り出す．
  string
  filename()
  {
    auto data = std::get<0>(GetParam());
    return string{TESTDATA_DIR} + "/" + data;
  }

  /// @brief テストパラメータからメソッド名を取り出す．
  string
  method()
  {
    return std::get<1>(GetParam());
  }

};


CondGenTestWithParam::CondGenTestWithParam()
{
}

// @brief 初期化を行う．
void
CondGenTestWithParam::SetUp()
{
}

// @brief 終了処理を行う．
void
CondGenTestWithParam::TearDown()
{
}

void
CondGenTestWithParam::do_test()
{
  unordered_map<string, JsonValue> option_dict;
  //option_dict.emplace("sat_param", JsonValue{"minisat2"});
  //option_dict.emplace("sat_param", JsonValue{"ymsat1_old"});
  option_dict.emplace("method", JsonValue{method()});
  JsonValue option(option_dict);
  auto network = TpgNetwork::read_blif(filename(), FaultType::StuckAt);

  SizeType limit = 1000;

  for ( auto ffr: network.ffr_list() ) {
    StructEngine engine(network, option);
    auto bd_enc = new BoolDiffEnc(engine, ffr->root(), option);
    engine.make_cnf({}, {ffr->root()});
    auto cond = CondGen::root_cond(network, ffr, limit, option);
    if ( cond.type() != DetCond::Detected ) {
      continue;
    }
    auto assumptions = CnfGenMgr::make_cnf(engine, cond, option);
    auto pvar = bd_enc->prop_var();
    auto assumptions1 = assumptions;
    assumptions1.push_back(~pvar);
    auto res = engine.solver().solve(assumptions1);
    if ( res != SatBool3::False ) {
      cout << "FFR#" << ffr->id() << endl;
      cond.print(cout);
      cout << "assumptions: ";
      for ( auto lit: assumptions ) {
	cout << " " << lit;
      }
      cout << endl;
      abort();
    }
    EXPECT_EQ( SatBool3::False, res );
  }
}

TEST_P(CondGenTestWithParam, test1)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(CondGenTest2, CondGenTestWithParam,
			 ::testing::Combine(::testing::ValuesIn(mydata),
					    ::testing::ValuesIn(mymethod)));

END_NAMESPACE_DRUID
