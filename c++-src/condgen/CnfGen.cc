
/// @file CnfGen.cc
/// @brief CnfGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "CnfGen.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
dfs(
  const AigHandle& aig,
  std::unordered_set<AigHandle>& mark,
  std::vector<SizeType>& input_id_list
)
{
  if ( aig.is_const() ) {
    return;
  }
  if ( mark.count(aig) > 0 ) {
    return;
  }
  mark.emplace(aig);

  if ( aig.is_input() ) {
    auto input_id = aig.input_id();
    input_id_list.push_back(input_id);
    return;
  }

  dfs(aig.fanin0(), mark, input_id_list);
  dfs(aig.fanin1(), mark, input_id_list);
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス CnfGen
//////////////////////////////////////////////////////////////////////

// @brief 条件を CNF に変換する．
std::vector<CnfGen::CondLits>
CnfGen::make_cnf(
  StructEngine& engine,
  const std::vector<AigHandle>& aig_list,
  const std::vector<SizeType>& ffr_id_list,
  const std::vector<BdInfo>& bd_list
)
{
  // aig_list 中に現れる入力番号のリストを得る．
  std::vector<SizeType> input_id_list;
  std::unordered_set<AigHandle> mark;
  for ( auto& aig: aig_list ) {
    dfs(aig, mark, input_id_list);
  }

  // 入力番号と SatLiteral の対応を表す辞書
  SatSolver::LitMap lit_map;
  for ( SizeType input_id: input_id_list ) {
    auto node_id = input_id / 2;
    auto time = input_id % 2;
    auto node = engine.network().node(node_id);
    auto as = Assign(node, time, true);
    auto lit = engine.conv_to_literal(as);
    lit_map.emplace(input_id, lit);
  }

  SizeType n = engine.network().ffr_num();
  std::vector<CondLits> lits_array(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    lits_array[i] = CondLits{i, false, {}};
  }

  // AIG を CNF に変換する．
  auto aig_lits_list = engine.solver().add_aig(aig_list, lit_map);
  // 結果を lits_array に反映させる．
  for ( SizeType i = 0; i < aig_list.size(); ++ i ) {
    auto& lits = aig_lits_list[i];
    auto id = ffr_id_list[i];
    lits_array[id] = CondLits{id, true, lits};
  }

  // ブール微分回路を作る．
  for ( auto& info: bd_list ) {
    auto id = info.id;
    auto bd_enc = new BoolDiffEnc(info.root, info.output_list);
    engine.add_subenc(std::unique_ptr<SubEnc>{bd_enc});
    auto plit = bd_enc->prop_var();
    auto& data = lits_array[id];
    if ( data.detected ) {
      auto old_lits = data.lits;
      if ( old_lits.size() == 1 ) {
	auto lit1 = old_lits.front();
	auto new_lit = engine.solver().new_variable(true);
	engine.solver().add_clause(~new_lit, lit1, plit);
	data.lits = {new_lit};
      }
      else {
	auto and_lit = engine.solver().new_variable(false);
	for ( auto lit: old_lits ) {
	  engine.solver().add_clause(~and_lit, lit);
	}
	auto new_lit = engine.solver().new_variable(true);
	engine.solver().add_clause(~new_lit, and_lit, plit);
	data.lits = {new_lit};
      }
    }
    else {
      lits_array[id] = CondLits{id, true, {plit}};
    }
  }

  return lits_array;
}

END_NAMESPACE_DRUID
