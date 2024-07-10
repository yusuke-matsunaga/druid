
/// @file FaultGroupGen.cc
/// @brief FaultGroupGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultGroupGen.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultGroupGen::FaultGroupGen(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mBaseEnc{network, option}
{
  auto& node_list = network.node_list();
  mBaseEnc.make_cnf(node_list, node_list);
}

// @brief デストラクタ
FaultGroupGen::~FaultGroupGen()
{
}

// @brief 両立故障グループを求める．
vector<vector<FaultInfo>>
FaultGroupGen::generate(
  const vector<FaultInfo>& fault_list
)
{
}

END_NAMESPACE_DRUID
