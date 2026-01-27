
/// @file cnfgen_test.cc
/// @brief cnfgen_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "types/FaultType.h"
#include "CondGen.h"
#include "condgen/CondGenMgr.h"
#include "condgen/CondGenStats.h"
#include "dtpg/BdEngine.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

std::string mydata[] = {
  "s27.blif",
  "s1196.blif",
  "s5378.blif",
  "s9234.blif"
};

std::string mymethod[] = {
  "naive",
  "cover",
  "factor",
  "aig"
};


class CondGenTestWithParam :
  public ::testing::TestWithParam<std::tuple<std::string, std::string>>
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
  std::string
  filename()
  {
    auto data_dir = std::filesystem::path{TESTDATA_DIR};
    auto name = std::get<0>(GetParam());
    return data_dir / name;
  }

  /// @brief テストパラメータからメソッド名を取り出す．
  std::string
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
  std::unordered_map<std::string, JsonValue> option_dict;
  //option_dict.emplace("sat_param", JsonValue{"minisat2"});
  //option_dict.emplace("sat_param", JsonValue{"ymsat1_old"});
  option_dict.emplace("method", JsonValue{method()});
  JsonValue option(option_dict);
  auto network = TpgNetwork::read_blif(filename(), FaultType::StuckAt);

  SizeType limit = 1000;

  for ( auto ffr: network.ffr_list() ) {
    BdEngine engine(network, ffr.root(), option);
    engine.add_prev_node(ffr.root());
    auto cond = CondGen::root_cond(network, ffr, limit, option);
    if ( cond.type() == DetCond::Undetected ) {
      continue;
    }
    CondGenStats stats;
    auto lits_list = CondGenMgr::make_cnf(engine, {cond}, option, stats);
    ASSERT_EQ( 1, lits_list.size() );
    auto& lits = lits_list.front();
    auto assumptions = lits;
    auto pvar = engine.prop_var();
    auto assumptions1 = assumptions;
    assumptions1.push_back(~pvar);
    auto res = engine.solver().solve(assumptions1);
    if ( res != SatBool3::False ) {
      std::cout << "FFR#" << ffr.id()
		<< std::endl;
      cond.print(std::cout);
      std::cout << "assumptions: ";
      for ( auto lit: assumptions ) {
	std::cout << " " << lit;
      }
      std::cout << std::endl;
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
