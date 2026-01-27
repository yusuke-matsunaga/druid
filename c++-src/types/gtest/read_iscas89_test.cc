
/// @file read_iscas89_test.cc
/// @brief read_iscas89_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"


BEGIN_NAMESPACE_DRUID

TEST(DruidTest, read_iscas89_1)
{
  std::string filename = "b01.bench";
  std::string path = std::string{TESTDATA_DIR} + "/" + filename;
  auto network = TpgNetwork::read_iscas89(path, FaultType::StuckAt);
  EXPECT_EQ( 54, network.node_num() );
  EXPECT_EQ( 2, network.input_num() );
  EXPECT_EQ( 2, network.output_num() );
  EXPECT_EQ( 7, network.ppi_num() );
  EXPECT_EQ( 7, network.ppo_num() );
  EXPECT_EQ( 21, network.mffc_num() );
  EXPECT_EQ( 24, network.ffr_num() );
  EXPECT_EQ( 5, network.dff_num() );
}

TEST(DruidTest, bad_read_iscas89_1)
{
  std::string path = "file_not_exist.bench";
  EXPECT_THROW( TpgNetwork::read_iscas89(path, FaultType::StuckAt),
		std::invalid_argument );
}

END_NAMESPACE_DRUID
