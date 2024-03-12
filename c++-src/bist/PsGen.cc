
/// @file PsGen.cc
/// @brief PsGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PsGen.h"


BEGIN_NAMESPACE_DRUID

// @brief Phase-Shifter を付加した回路を生成する．
TpgNetwork
PsGen::generate(
  const TpgNetwork& src_network,
  SizeType lfsr_bitlen,
  const vector<vector<SizeType>>& input_config
)
{
  SizeType NI = src_network.ppi_num();
  if ( NI != input_config.size() ) {
    ostringstream buf;
    buf << "input_config.size() does not match with src_network.ppi_num()";
    throw std::invalid_argument{buf.str()};
  }
}

END_NAMESPACE_DRUID
