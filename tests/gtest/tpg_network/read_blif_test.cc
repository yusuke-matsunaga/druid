
/// @file read_blif_test.cc
/// @brief read_blif_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_DRUID

TEST(DruidTest, read_blif_1)
{
  string filename = "s38584.blif";
  string path = DATAPATH + filename;
  auto network = TpgNetwork::read_blif(path);

  EXPECT_EQ( 23900, network.node_num() );
  EXPECT_EQ( 13, network.input_num() );
  EXPECT_EQ( 278, network.output_num() );
  EXPECT_EQ( 1465, network.ppi_num() );
  EXPECT_EQ( 1730, network.ppo_num() );
  EXPECT_EQ( 4689, network.mffc_num() );
  EXPECT_EQ( 5676, network.ffr_num() );
  EXPECT_EQ( 1452, network.dff_num() );

  auto nn = network.node_num();
  for ( SizeType i = 0; i < nn; ++ i ) {
    auto node = network.node(i);
    EXPECT_EQ( i, node->id() );
  }

  auto ni = network.input_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto node = network.input(i);
    EXPECT_EQ( i, node->input_id() );
  }

  auto no = network.output_num();
  for ( SizeType i = 0; i < no; ++ i ) {
    auto node = network.output(i);
    EXPECT_EQ( i, node->output_id() );
  }
  for ( SizeType i = 0; i < no; ++ i ) {
    auto node = network.output2(i);
    EXPECT_EQ( i, node->output_id2() );
  }

  auto npi = network.ppi_num();
  for ( SizeType i = 0; i < npi; ++ i ) {
    auto node = network.ppi(i);
    EXPECT_EQ( i, node->input_id() );
  }

  auto npo = network.ppo_num();
  for ( SizeType i = 0; i < npo; ++ i ) {
    auto node = network.ppo(i);
    EXPECT_EQ( i, node->output_id() );
  }

  auto nmffc = network.mffc_num();
  for ( SizeType i = 0; i < nmffc; ++ i ) {
    auto mffc = network.mffc(i);
    EXPECT_EQ( i, mffc.id() );
  }

  auto nffr = network.ffr_num();
  for ( SizeType i = 0; i < nffr; ++ i ) {
    auto ffr = network.ffr(i);
    EXPECT_EQ( i, ffr.id() );
  }

  auto nd = network.dff_num();
  for ( SizeType i = 0; i < nd; ++ i ) {
    auto& dff = network.dff(i);
    EXPECT_EQ( i, dff.id() );
  }
}

TEST(DruidTest, bad_read_blif_1)
{
  string path = "file_not_exist.blif";
  EXPECT_THROW( {
      auto _ = TpgNetwork::read_blif(path);
    }, std::invalid_argument );
}

END_NAMESPACE_DRUID
