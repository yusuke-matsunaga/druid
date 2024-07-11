
/// @file TestCubeGen.cc
/// @brief TestCubeGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "TestCubeGen.h"
#include "ExCubeGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

// @brief 与えられた故障を検出するテストキューブを生成する．
void
TestCubeGen::run(
  const TpgNetwork& network,
  vector<FaultInfo>& fault_list,
  const JsonValue& option
)
{
  // FFR ごとにSATソルバを作る．

  // FFR の根のノード番号と FFR 番号の対応関係を表す辞書を作る．
  unordered_map<SizeType, SizeType> ffr_map;
  for ( auto ffr: network.ffr_list() ) {
    auto root = ffr->root();
    ffr_map.emplace(root->id(), ffr->id());
  }
  // fault_list に含まれる故障を FFR ごとに分割する．
  SizeType nffr = network.ffr_num();
  vector<vector<FaultInfo*>> ffr_fault_list(nffr);
  for ( auto& finfo: fault_list ) {
    if ( finfo.is_trivial() ) {
      continue;
    }
    auto fault = finfo.fault();
    auto root = fault->ffr_root();
    auto id = ffr_map.at(root->id());
    ffr_fault_list[id].push_back(&finfo);
  }

  for ( auto ffr: network.ffr_list() ) {
    ExCubeGen gen{network, ffr, option};
    for ( auto finfo: ffr_fault_list[ffr->id()] ) {
      gen.run(*finfo);
    }
  }
}

END_NAMESPACE_DRUID
