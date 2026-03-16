
/// @file ExtStd.cc
/// @brief ExtStd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "ExtStd.h"
#include "ExData.h"
#include "MkBdd.h"
#include "dtpg/SuffCond.h"
#include "ym/BddLit.h"
#include "ym/BddCube.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExtStd
//////////////////////////////////////////////////////////////////////

// @brief 指定された出力からバックトレースを行う．
SuffCond
ExtStd::backtrace(
  const ExData& data,
  const TpgNode& output
)
{
  std::vector<std::vector<TpgNode>> choice_list;
  std::vector<TpgNode> aux_side_inputs;
  auto node_list = data.backtrace(output, choice_list, aux_side_inputs);
  auto cnode_list = select_cnode(choice_list);
  node_list.insert(node_list.end(), cnode_list.begin(), cnode_list.end());

  // AssignList に変換する．
  AssignList assign_list;
  for ( auto& node: node_list ) {
    auto bval = (data.gval(node) == Val3::_1);
    assign_list.add(node, 1, bval);
  }

  // aux_side_inputs の解析を行う．
  MkBdd mk_bdd(data, node_list, aux_side_inputs);

  auto f = mk_bdd.make_bdd(output);
  auto root_var = mk_bdd.root_var();
  auto f0 = f.cofactor(root_var, true);
  auto f1 = f.cofactor(root_var, false);
  auto f_diff = f0 ^ f1;

  // aux_side_inputs のうち束縛しなくてよい変数を探す
  auto n = aux_side_inputs.size();
  AssignList aux_side_inputs2;
  std::vector<bool> mark(n, true);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& node = aux_side_inputs[i];
    auto val = data.gval(node) == Val3::_1;
    mark[i] = false;
    BddCube cube;
    for ( SizeType j = 0; j < n; ++ j ) {
      if ( mark[j] ) {
	auto var = mk_bdd.aux_side_input_var(j);
	bool inv = ( data.gval(node) == Val3::_0 );
	auto lit = BddLit(var, inv);
	cube &= lit;
      }
    }
    auto f2 = f_diff / cube;
    if ( !f2.is_one() ) {
      // i は束縛する必要がある．
      mark[i] = true;
      assign_list.add(node, 1, val);
    }
    else {
      aux_side_inputs2.add(node, 1, val);
    }
  }

  return SuffCond(assign_list, aux_side_inputs2);
}

// @brief 制御値を持つ side input を選ぶ．
std::vector<TpgNode>
ExtStd::select_cnode(
  const std::vector<std::vector<TpgNode>>& choice_list
)
{
  // 出現回数を数える．
  std::unordered_map<SizeType, SizeType> node_count;
  for ( auto& cnode_list: choice_list ) {
    for ( auto& cnode: cnode_list ) {
      if ( node_count.count(cnode.id()) == 0 ) {
	node_count.emplace(cnode.id(), 1);
      }
      else {
	++ node_count.at(cnode.id());
      }
    }
  }

  // 出現回数の多い順に選択する．
  std::vector<TpgNode> ans_list;
  std::unordered_set<SizeType> selected;
  for ( auto& cnode_list: choice_list ) {
    SizeType max_count = 0;
    TpgNode max_node;
    bool done = false;
    for ( auto& cnode: cnode_list ) {
      if ( selected.count(cnode.id()) > 0 ) {
	done = true;
	break;
      }
      SizeType count = node_count.at(cnode.id());
      if ( max_count < count ) {
	max_count = count;
	max_node = cnode;
      }
    }
    if ( !done ) {
      ans_list.push_back(max_node);
      selected.emplace(max_node.id());
    }
  }

  return ans_list;
}

END_NAMESPACE_DRUID
