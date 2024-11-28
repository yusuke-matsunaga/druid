
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "ExData.h"
#include "AssignMgr.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// Extractor の継承クラスを作る．
Extractor*
Extractor::new_impl(
  const JsonValue& option
)
{
  if ( option.is_null() ) {
    // デフォルトフォールバック
    return new ExtSimple;
  }
  if ( option.is_string() ) {
    auto mode = option.get_string();
    if ( mode == "simple" ) {
      return new ExtSimple;
    }
    // 知らない型だった．
    ostringstream buf;
    buf << mode << ": unknown value for 'extractor'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  ostringstream buf;
  buf << "value for 'extractor' should be a string or null";
  throw std::invalid_argument{buf.str()};
  // ダミー
  return nullptr;
}

// @brief 値割り当てを１つ求める．
AssignList
Extractor::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( !data.sensitized_output_list().empty() );

  AssignList min_assign_list;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  for ( auto po: data.sensitized_output_list() ) {
    vector<vector<const TpgNode*>> choice_list;
    auto node_list = data.backtrace(po, choice_list);
    auto cnode_list = select_cnode(choice_list);
    node_list.insert(node_list.end(), cnode_list.begin(), cnode_list.end());
    // AssignList に変換する．
    AssignList assign_list;
    for ( auto node: node_list ) {
      auto bval = (data.gval(node) == Val3::_1);
      assign_list.add(node, 1, bval);
    }
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      min_val = val;
      min_assign_list = assign_list;
    }
  }
  return min_assign_list;
}

BEGIN_NONAMESPACE

// 共通なノードのリストを作る．
//
// と同時に node_list_array から共通なノードを取り除く
vector<const TpgNode*>
common_node(
  vector<vector<const TpgNode*>>& node_list_array
)
{
  // node_list_array[i] の要素は node->id() でソートされている
  // と仮定している．
  auto n = node_list_array.size();
  ASSERT_COND ( n > 0 );
  vector<const TpgNode*> node_list{node_list_array[0]};
  for ( SizeType i = 1; i < n; ++ i ) {
    auto rpos = node_list.begin();
    auto wpos = rpos;
    auto epos = node_list.end();
    auto rpos2 = node_list_array[i].begin();
    auto epos2 = node_list_array[i].end();
    while ( rpos != epos && rpos2 != epos2 ) {
      auto node = *rpos;
      auto node2 = *rpos2;
      if ( node->id() < node2->id() ) {
	++ rpos;
      }
      else if ( node->id() > node2->id() ) {
	++ rpos2;
      }
      else {
	if ( rpos != wpos ) {
	  *wpos = node;
	}
	++ rpos;
	++ wpos;
	++ rpos2;
      }
    }
    if ( wpos != epos ) {
      node_list.erase(wpos, epos);
    }
    if ( node_list.empty() ) {
      return node_list;
    }
  }
  // node_list_array の更新を行う．
  std::unordered_set<SizeType> node_set;
  for ( auto node: node_list ) {
    node_set.emplace(node->id());
  }
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& node_list1 = node_list_array[i];
    auto rpos = node_list1.begin();
    auto epos = node_list1.end();
    auto wpos = rpos;
    for ( ; rpos != epos; ++ rpos ) {
      auto node = *rpos;
      if ( node_set.count(node->id()) > 0 ) {
	continue;
      }
      if ( rpos != wpos ) {
	*wpos = node;
      }
      ++ wpos;
    }
    if ( wpos != epos ) {
      node_list1.erase(wpos, epos);
    }
  }
  return node_list;
}

END_NONAMESPACE

// @brief 各出力へ故障伝搬する値割り当てを求める．
AssignExpr
Extractor::extract_all(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  // 最終的には一つの Expr にするが，共通部分を求めたいので
  // AND/OR を分けて持っておく．
  vector<vector<const TpgNode*>> node_list_array;
  vector<vector<vector<const TpgNode*>>> choice_node_list_array;
  for ( auto po: data.sensitized_output_list() ) {
    vector<vector<const TpgNode*>> choice_node_list;
    auto node_list = data.backtrace(po, choice_node_list);
    node_list_array.push_back(node_list);
    choice_node_list_array.push_back(choice_node_list);
  }

  // node_list_array の共通部分を求める．
  auto common_node_list = common_node(node_list_array);

  // これらの結果を Expr に変換する．
  AssignMgr assign_mgr;
  // 共通な条件
  Expr common_cube = Expr::one();
  for ( auto node: common_node_list ) {
    auto inv = (data.gval(node) == Val3::_0);
    auto var = assign_mgr.get_varid(Assign{node, 1, true});
    auto lit = Expr::literal(var, inv);
    common_cube &= lit;
  }
  // 個々の出力ごとの条件
  vector<Expr> cond_list;
  auto n = node_list_array.size();
  cond_list.reserve(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& node_list = node_list_array[i];
    auto& cgroup_list = choice_node_list_array[i];
    Expr cond = Expr::one();
    for ( auto node: node_list ) {
      auto inv = (data.gval(node) == Val3::_0);
      auto var = assign_mgr.get_varid(Assign{node, 1, true});
      auto lit = Expr::literal(var, inv);
      cond &= lit;
    }
    for ( auto cgroup: cgroup_list ) {
      Expr clause = Expr::zero();
      for ( auto node: cgroup ) {
	auto inv = (data.gval(node) == Val3::_0);
	auto var = assign_mgr.get_varid(Assign{node, 1, true});
	auto lit = Expr::literal(var, inv);
	clause |= lit;
      }
      cond &= clause;
    }
    cond_list.push_back(cond);
  }
  auto or_cond = Expr::or_op(cond_list);
  auto cond = common_cube & or_cond;
  return AssignExpr{cond, assign_mgr.assign_list()};
}

END_NAMESPACE_DRUID
