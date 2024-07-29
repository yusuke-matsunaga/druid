
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "ExData.h"


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

  mData = &data;

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( !mData->sensitized_output_list().empty() );
  AssignList min_assign_list;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  for ( auto po: sensitized_output_list() ) {
    vector<vector<const TpgNode*>> choice_list;
    auto assign_list = backtrace(po, choice_list);
    auto cnode_list = select_cnode(choice_list);
    for ( auto cnode: cnode_list ) {
      record_side_val(cnode, assign_list);
    }
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      min_val = val;
      min_assign_list = assign_list;
    }
  }
  return min_assign_list;
}

// @brief 故障差の伝搬している経路の side input の値を記録する．
AssignList
Extractor::backtrace(
  const TpgNode* po,
  vector<vector<const TpgNode*>>& choice_list
)
{
  // その経路の side input の値を記録する．
  AssignList assign_list;
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
      // 故障の影響が伝搬しているノードの場合
      record_sensitized_node(node, assign_list);
      break;
    case 2:
      // 故障の影響が伝搬していないノードの場合
      record_masking_node(node, assign_list, choice_list);
      break;
    }
  }

  if ( debug ) {
    DBG_OUT << "Extract at ";
    const char* comma = "";
    DBG_OUT << comma << "Node#" << root()->id() << endl;
    DBG_OUT << "assign_list = " << assign_list << endl;
    SizeType cid = 0;
    for ( auto& cnode_list: choice_list ) {
      DBG_OUT << "Choice#" << (cid + 1)
	      << ": [";
      const char* spc = "";
      for ( auto cnode: cnode_list ) {
	DBG_OUT << spc << "Node#" << cnode->id();
	spc = " ";
      }
      DBG_OUT << endl;
      ++ cid;
    }
  }

  return assign_list;
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
Extractor::record_sensitized_node(
  const TpgNode* node,
  AssignList& assign_list
)
{
  if ( debug ) {
    DBG_OUT << "record_sinsitized_node" << endl
	    << gval(node) << " / " << fval(node) << endl;
  }
  for ( auto inode: node->fanin_list() ) {
    // 値に応じてタイプ分けを行う．
    // 実は XOR の side-input は X でよいが
    // なにも考えずに現在の値を要求している．
    int t = type(inode);
    if ( t == 3 ) {
      record_side_val(inode, assign_list);
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
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
Extractor::record_masking_node(
  const TpgNode* node,
  AssignList& assign_list,
  vector<vector<const TpgNode*>>& choice_list
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
      record_side_val(cnode, assign_list);
    }
    else {
      choice_list.push_back(cnode_list);
    }
  }
  else {
    // ここに来たということは全てのファンインに故障差が伝搬していないか
    // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
    // に再帰する．
    record_sensitized_node(node, assign_list);
  }
}

// @brief side input の値を記録する．
void
Extractor::record_side_val(
  const TpgNode* node,
  AssignList& assign_list
)
{
  bool val = (gval(node) == Val3::_1);
  assign_list.add(node, 1, val);
}

END_NAMESPACE_DRUID
