
/// @file condgen_test.cc
/// @brief condgen_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "dtpg/DtpgMgr.h"
#include "types/TpgNetwork.h"
#include "types/TpgFFR.h"
#include "condgen/CondGenMgr.h"
#include "CondGen.h"
#include "CondGenChecker.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_DRUID

std::string mydata[] = {
  "s27.blif",
  "s1196.blif",
  "s5378.blif",
  "s9234.blif"
};


class CondGenTestWithParam :
  public ::testing::TestWithParam<std::tuple<std::string, FaultType>>
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
  filename();

  /// @brief テストパラメータから FaultType を取り出す．
  FaultType
  fault_type();

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
  JsonValue option{option_dict};
  auto network = TpgNetwork::read_blif(filename(), fault_type());

  SizeType limit = 1000;
  for ( auto ffr: network.ffr_list() ) {
    auto cond = CondGen::root_cond(network, ffr, limit, option);
    if ( cond.type() == DetCond::Undetected ) {
      continue;
    }
    CondGenChecker checker(network, cond);
    auto& engine = checker.engine();
    auto& solver = checker.solver();
    auto lit1 = checker.lit1();
    auto lit2 = checker.lit2();
    if ( lit1 == SatLiteral::X ) {
      auto res1 = solver.solve({~lit2});
      EXPECT_EQ( SatBool3::False, res1 );
      if ( res1 != SatBool3::False ) {
	cond.print(std::cout);
      }
    }
    else {
      auto res1 = solver.solve({~lit1, lit2});
      EXPECT_EQ( SatBool3::False, res1 );
      if ( res1 != SatBool3::False ) {
	cond.print(std::cout);
	auto& model = solver.model();
	auto n = engine.output_list().size();
	for ( SizeType i = 0; i < n; ++ i ) {
	  auto plit = engine.prop_var(i);
	  if ( model[plit] == SatBool3::True ) {
	    auto output = engine.output_list()[i];
	    std::cout << "detected at output#" << i << ": " << output.id()
		      << std::endl;
	  }
	}
      }
      auto res2 = solver.solve({lit1, ~lit2});
      EXPECT_EQ( SatBool3::False, res2 );
      if ( res2 != SatBool3::False ) {
	cond.print(std::cout);
      }
    }
  }
}

// @brief テストパラメータからファイル名を取り出す．
std::string
CondGenTestWithParam::filename()
{
  auto data_dir = std::filesystem::path{TESTDATA_DIR};
  auto name = std::get<0>(GetParam());
  return data_dir / name;
}

// @brief テストパラメータから FaultType を取り出す．
FaultType
CondGenTestWithParam::fault_type()
{
  return std::get<1>(GetParam());
}

TEST_P(CondGenTestWithParam, test2)
{
  do_test();
}

INSTANTIATE_TEST_SUITE_P(CondGenTest2, CondGenTestWithParam,
			 ::testing::Combine(::testing::ValuesIn(mydata),
					    ::testing::Values(FaultType::StuckAt,
							      FaultType::TransitionDelay)));

END_NAMESPACE_DRUID
