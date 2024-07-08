
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
  SizeType cube_per_fault,
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
    auto fault = finfo.fault();
    auto root = fault->ffr_root();
    auto id = ffr_map.at(root->id());
    ffr_fault_list[id].push_back(finfo);
  }

  vector<TestVector> tv_list;
  for ( auto ffr: network.ffr_list() ) {
    ExCubeGen gen{network, ffr, cube_per_fault, option};
    for ( auto& finfo: ffr_fault_list[ffr->id()] ) {
      gen.run(finfo);
    }
  }

  return tv_list;
}

// 故障に対するテストキューブを複数作る．
vector<TestVector>
testcube_gen2(
  const TpgNetwork& network,
  const vector<FaultInfo>& fault_list,
  SizeType cube_per_fault,
  const JsonValue& option
)
{
  vector<TestVector> tv_list;
  // 故障ごとに使い捨てのSATソルバを作る．
  for ( auto& finfo: fault_list ) {
    auto fault = finfo.fault();
    auto node = fault->ffr_root();
    BaseEnc base_enc{network, option};
    auto bd_enc = new BoolDiffEnc{base_enc, node, option};
    base_enc.make_cnf({}, {node});
    auto ffr_cond = fault->ffr_propagate_condition();
    auto assumptions = base_enc.conv_to_literal_list(ffr_cond);
    auto res = base_enc.solver().solve(assumptions);
    ASSERT_COND( res == SatBool3::True );
    auto suf_cond = bd_enc->extract_sufficient_condition();
    auto pi_assign = base_enc.justify(suf_cond);
    auto tv = TestVector{network, pi_assign};
    tv_list.push_back(tv);
    for ( SizeType i = 1; i < cube_per_fault; ++ i ) {
      // tv を否定した節を追加
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(pi_assign.size());
      for ( auto nv: pi_assign ) {
	auto lit = base_enc.conv_to_literal(nv);
	tmp_lits.push_back(~lit);
      }
      base_enc.solver().add_clause(tmp_lits);
      auto res = base_enc.solver().solve(assumptions);
      if ( res != SatBool3::True ) {
	// UNSAT になったら終わり
	break;
      }
      auto suf_cond = bd_enc->extract_sufficient_condition();
      pi_assign = base_enc.justify(suf_cond);
      auto tv = TestVector{network, pi_assign};
      tv_list.push_back(tv);
    }
  }
  return tv_list;
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
  JsonValue dtpg_option;
  SizeType cube_per_fault = 1;
  if ( option.is_object() ) {
    if ( option.has_key("cube_per_fault") ) {
      auto val = option.get("cube_per_fault");
      cube_per_fault = val.get_int();
    }
    if ( option.has_key("dtpg") ) {
      dtpg_option = option.get("dtpg");
    }
  }
  vector<TestVector> tv_list;
  tv_list = testcube_gen1(network, fault_list, cube_per_fault, dtpg_option);

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
