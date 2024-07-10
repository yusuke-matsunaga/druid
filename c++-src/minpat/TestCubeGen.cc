
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
#include "TestVector.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// 故障に対するテストキューブを一つづつ作る．
vector<TestVector>
testcube_gen1(
  const TpgNetwork& network,
  const vector<FaultInfo>& fault_list,
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
  vector<vector<FaultInfo>> ffr_fault_list(nffr);
  for ( auto& finfo: fault_list ) {
    if ( finfo.is_trivial() ) {
      continue;
    }
    auto fault = finfo.fault();
    auto root = fault->ffr_root();
    auto id = ffr_map.at(root->id());
    ffr_fault_list[id].push_back(finfo);
  }

  for ( auto ffr: network.ffr_list() ) {
    ExCubeGen gen{network, ffr, option};
    for ( auto& finfo: ffr_fault_list[ffr->id()] ) {
      gen.run(finfo);
    }
  }
}

END_NONAMESPACE

// @brief 与えられた故障を検出するテストキューブを生成する．
vector<TestVector>
TestCubeGen::run(
  const TpgNetwork& network,
  const vector<FaultInfo>& fault_list,
  const JsonValue& option
)
{
  vector<TestVector> tv_list;
  tv_list = testcube_gen1(network, fault_list, option);

  // 同一のパタンを取り除く．
  SizeType n0 = tv_list.size();
  sort(tv_list.begin(), tv_list.end(),
       [](const TestVector& a, const TestVector& b)->bool{
	 auto a_str = a.hex_str();
	 auto b_str = b.hex_str();
	 return a_str < b_str;
       });
  auto rpos = tv_list.begin();
  auto end = tv_list.end();
  auto wpos = tv_list.begin();
  TestVector prev_tv;
  for ( ; rpos != end; ++ rpos ) {
    auto cur_tv = *rpos;
    if ( cur_tv != prev_tv ) {
      if ( rpos != wpos ) {
	*wpos = cur_tv;
      }
      ++ wpos;
      prev_tv = cur_tv;
    }
  }
  if ( wpos != end ) {
    tv_list.erase(wpos, end);
  }

  cout << n0 << " -> " << tv_list.size() << endl;

  return tv_list;
}

END_NAMESPACE_DRUID
