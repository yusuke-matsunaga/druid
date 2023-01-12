
/// @file read_blif_test.cc
/// @brief read_blif_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

TEST(DruidTest, read_blif_1)
{
  string filename = "s38584.blif";
  string path = DATAPATH + filename;
  auto network = TpgNetwork::read_blif(path);
}

TEST(DruidTest, bad_read_blif_1)
{
  string path = "file_not_exist.blif";
  EXPECT_THROW( {
      auto _ = TpgNetwork::read_blif(path);
    }, std::invalid_argument );
}

END_NAMESPACE_DRUID
