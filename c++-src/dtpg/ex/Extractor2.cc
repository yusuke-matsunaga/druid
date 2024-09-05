
/// @file Extractor2.cc
/// @brief Extractor2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor2.h"
#include "ExData.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = 0;


END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor2
//////////////////////////////////////////////////////////////////////

// @brief 値割り当てを１つ求める．
AssignExpr
Extractor2::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  mData = &data;

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( !mData->sensitized_output_list().empty() );
  vector<AssignExpr> assign_list;
  for ( auto po: sensitized_output_list() ) {
    auto assign1 = backtrace(po);
    assign_list.push_back(assign1);
  }
  return AssignExpr::make_or(assign_list);
}

// @brief 故障差の伝搬状況を調べるためのDFSを行う．
bool
Extractor2::dfs(
  const TpgNode* node
)
{
}

// @brief 故障差の伝搬している経路の side input の値を記録する．
AssignExpr
Extractor2::backtrace(
  const TpgNode* po
)
{
  // その経路の side input の値を記録する．
  vector<AssignExpr> assign_list;
  clear_queue();
  put_queue(po);
  while ( !mQueue.empty() ) {
    auto node = get_queue();
    if ( node == root() ) {
      continue;
    }
    auto t = type(node);
    if ( debug ) {
      DBG_OUT << "visit at Node#" << node->id()
	      << ": " << t << endl;
    }
    switch ( t ) {
    case 1:
      { // 故障の影響が伝搬しているノードの場合
	auto assign1 = record_sensitized_node(node);
	assign_list.push_back(assign1);
      }
      break;
    case 2:
      { // 故障の影響が伝搬していないノードの場合
	auto assign1 = record_masking_node(node);
	assign_list.push_back(assign1);
      }
      break;
    }
  }
  auto assign_expr = AssignExpr::make_and(assign_list);

  if ( debug ) {
    DBG_OUT << "Extract at ";
    DBG_OUT << "Node#" << root()->id() << endl;
    DBG_OUT << "assign_expr = " << assign_expr << endl;
  }

  return assign_expr;
}

// @brief 故障の影響の伝搬させる値割当を記録する．
AssignExpr
Extractor2::record_sensitized_node(
  const TpgNode* node
)
{
  if ( debug ) {
    DBG_OUT << "record_sinsitized_node" << endl
	    << gval(node) << " / " << fval(node) << endl;
  }
  vector<AssignExpr> assign_list;
  for ( auto inode: node->fanin_list() ) {
    // 値に応じてタイプ分けを行う．
    int t = type(inode);
    if ( t == 3 ) {
      // focone の外のノードの場合
      // 今の値を記録する．
      // 実は XOR の side-input は X でよいが
      // なにも考えずに現在の値を要求している．
      auto assign1 = record_side_val(inode);
      assign_list.push_back(assign1);
    }
    else {
      put_queue(inode);
    }
    // 正確には
    // 1. 故障差の伝搬している入力はすべてその値を確定させる．
    // 2. その他の入力で伝搬に必要な値を固定する．
    if ( debug ) {
      DBG_OUT << "  Node#" << inode->id()
	      << " type = " << t
	      << " " << gval(inode) << " / " << fval(inode) << endl;
    }
  }
  return AssignExpr::make_and(assign_list);
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
AssignExpr
Extractor2::record_masking_node(
  const TpgNode* node
)
{
  if ( debug ) {
    DBG_OUT << "record_masking_node" << endl
	    << gval(node) << " / " << fval(node) << endl;
  }
  bool has_snode = false;
  vector<const TpgNode*> cnode_list;
  for ( auto inode: node->fanin_list() ) {
    int t = type(inode);
    if ( t == 1 ) {
      // このノードには故障差が伝搬している．
      has_snode = true;
    }
    else if ( t == 3 ) {
      if ( node->cval() == gval(inode) ) {
	// このノードは制御値を持っている．
	cnode_list.push_back(inode);
      }
    }
  }
  if ( has_snode && cnode_list.size() > 0 ) {
    // node のファンインに故障差が伝搬しており，
    // 他のファンインの制御値でブロックされている場合，
    // その制御値を持つノードの値を確定させる．
    // 制御値を持つファンインが2つ以上ある場合には
    // 異なる結果になる可能性がある．
    if ( cnode_list.size() == 1 ) {
      auto cnode = cnode_list.front();
      return record_side_val(cnode);
    }
    else {
      vector<AssignExpr> assign_list;
      for ( auto cnode: cnode_list ) {
	auto assign1 = record_side_val(cnode);
	assign_list.push_back(assign1);
      }
      return AssignExpr::make_or(assign_list);
    }
  }
  else {
    // ここに来たということは全てのファンインに故障差が伝搬していないか
    // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
    // に再帰する．
    return record_sensitized_node(node);
  }
}

// @brief side input の値を記録する．
AssignExpr
Extractor2::record_side_val(
  const TpgNode* node
)
{
  bool val = (gval(node) == Val3::_1);
  return AssignExpr::make_literal(Assign{node, 1, val});
}

// @brief AssignExpr のストリーム出力演算子
ostream&
operator<<(
  ostream& s,
  const AssignExpr& expr
)
{
  if ( expr.is_literal() ) {
    auto assign = expr.literal();
    s << assign;
  }
  else if ( expr.is_and() ) {
    auto& opr_list = expr.opr_list();
    s << "(";
    const char* op = "";
    for ( auto& expr: opr_list ) {
      s << op << expr;
      op = " & ";
    }
    s << ")";
  }
  else if ( expr.is_or() ) {
    auto& opr_list = expr.opr_list();
    s << "(";
    const char* op = "";
    for ( auto& expr: opr_list ) {
      s << op << expr;
      op = " | ";
    }
    s << ")";
  }
  return s;
}

END_NAMESPACE_DRUID
