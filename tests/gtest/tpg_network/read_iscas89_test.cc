
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
}

TEST(DruidTest, bad_read_iscas89_1)
{
  string path = "file_not_exist.bench";
  EXPECT_THROW( {
      auto _ = TpgNetwork::read_iscas89(path);
    }, std::invalid_argument );
}

END_NAMESPACE_DRUID
