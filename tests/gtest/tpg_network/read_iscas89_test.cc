
/// @file read_iscas89_test.cc
/// @brief read_iscas89_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

TEST(DruidTest, read_iscas89_1)
{
  string filename = "b01.bench";
  string path = DATAPATH + filename;
  auto network = TpgNetwork::read_iscas89(path);
  EXPECT_EQ( 60, network.node_num() );
  EXPECT_EQ( 3, network.input_num() );
  EXPECT_EQ( 2, network.output_num() );
  EXPECT_EQ( 8, network.ppi_num() );
  EXPECT_EQ( 7, network.ppo_num() );
  EXPECT_EQ( 21, network.mffc_num() );
  EXPECT_EQ( 24, network.ffr_num() );
  EXPECT_EQ( 5, network.dff_num() );
}

TEST(DruidTest, bad_read_iscas89_1)
{
  string path = "file_not_exist.bench";
  EXPECT_THROW( {
      auto _ = TpgNetwork::read_iscas89(path);
    }, std::invalid_argument );
}

END_NAMESPACE_DRUID
