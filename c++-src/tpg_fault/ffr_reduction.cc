
/// @file ffr_reduction.cc
/// @brief ffr_reduction の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFaultImpl.h"
#include "NodeValList.h"
#include "ym/SatSolver.h"
#include "VidMap.h"
#include "GateEnc.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

void
make_cnf(
  SatSolver& solver,
  const TpgFFR& ffr,
  VidMap& varmap
)
{
  // ffr の入力の変数を作る．
  for ( auto node: ffr.input_list() ) {
    auto var = solver.new_variable(true);
    varmap.set_vid(node, var);
  }

  // ffr のノードの変数を作る．
  for ( auto node: ffr.node_list() ) {
    auto var = solver.new_variable(true);
    varmap.set_vid(node, var);
  }

  // ffr のノードの関係を表すCNFを作る．
  GateEnc enc{solver, varmap};
  for ( auto node: ffr.node_list() ) {
    enc.make_cnf(node);
  }
}

vector<SatLiteral>
ffr_propagate_condition(
  TpgFaultImpl* fault,
  VidMap& varmap
)
{
  vector<SatLiteral> ans_list;
  auto ex_cond = fault->excitation_condition();
  for ( auto nv: ex_cond ) {
    auto node = nv.node();
    auto time = nv.time();
    auto inv = !nv.val();
    auto lit = varmap(node) * inv;
    //cout << node->str() << ": " << inv << " => " << lit << endl;
    ans_list.push_back(lit);
  }
  for ( auto node = fault->origin_node(); node->fanout_num() == 1; ) {
    auto fonode = node->fanout(0);
    auto val = fonode->side_val();
    if ( val != Val3::_X ) {
      bool inv = val == Val3::_0 ? true : false;
      for ( auto inode: fonode->fanin_list() ) {
	if ( inode != node ) {
	  auto lit = varmap(inode) * inv;
	  ans_list.push_back(lit);
	}
      }
    }
    node = fonode;
  }
  return ans_list;
}

void
reduce(
  const TpgFFR& ffr,
  const vector<TpgFaultImpl*>& fault_list,
  unordered_set<SizeType>& rep_map
)
{
  // 関係するノード番号の最大値を得る．
  SizeType max_id = 0;
  for ( auto node: ffr.input_list() ) {
    max_id = std::max(max_id, node->id());
  }
  for ( auto node: ffr.node_list() ) {
    max_id = std::max(max_id, node->id());
  }
  ++ max_id;

  // SATソルバ
  SatSolver solver{SatSolverType{"ymsat2"}};

  // FFRに関する関係を表すCNFを作る．
  VidMap varmap{max_id};
  make_cnf(solver, ffr, varmap);

  // ナイーブな方法 O(n^2) のアルゴリズム
  SizeType n = fault_list.size();
  for ( SizeType i1 = 0; i1 < n; ++ i1 ) {
    auto f1 = fault_list[i1];
    if ( rep_map.count(f1->id()) == 0 ) {
      continue;
    }

    cout << "f1 = " << f1->str() << endl;

    // f1 の検出条件
    auto f1_cond = ffr_propagate_condition(f1, varmap);

    for ( SizeType i2 = 0; i2 < n; ++ i2 ) {
      if ( i1 == i2 ) {
	continue;
      }
      auto f2 = fault_list[i2];
      if ( rep_map.count(f2->id()) == 0 ) {
	continue;
      }

      cout << "f2 = " << f2->str() << endl;

      // f2 の検出条件
      auto f2_cond = ffr_propagate_condition(f2, varmap);

      bool unsat = true;
      for ( auto lit: f2_cond ) {
	vector<SatLiteral> assumptions = f1_cond;
	assumptions.push_back(~lit);
	auto ret = solver.solve(assumptions);
	if ( ret != SatBool3::False ) {
	  unsat = false;
	  break;
	}
      }
      if ( unsat ) {
	// UNSAT だった -> f1 は f2 を支配している．
	f2->add_dom_fault(f1);
      }
    }
  }
}

void
ffr_reduction(
  const TpgNetwork& network,
  const vector<TpgFaultImpl*>& fault_array,
  unordered_set<SizeType>& rep_map
)
{
  // ノード単位の故障リストを作る．
  vector<vector<SizeType>> node_fault_list(network.node_num());
  for ( auto f: fault_array ) {
    SizeType fid = f->id();
    if ( rep_map.count(fid) > 0 ) {
      auto node = f->origin_node();
      node_fault_list[node->id()].push_back(fid);
    }
  }

  // FFR 単位で故障の縮約を行う．
  for ( auto ffr: network.ffr_list() ) {
    vector<TpgFaultImpl*> fault_list;
    for ( auto node: ffr.node_list() ) {
      for ( auto fid: node_fault_list[node->id()] ) {
	auto fault = fault_array[fid];
	fault_list.push_back(fault);
      }
    }
    reduce(ffr, fault_list, rep_map);
  }
}

END_NAMESPACE_DRUID
