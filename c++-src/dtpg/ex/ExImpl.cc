
/// @file ExImpl.cc
/// @brief ExImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ExImpl.h"
#include "ExData.h"
#include "TpgNode.h"
#include "ym/JsonValue.h"


#define DBG_OUT cout

BEGIN_NONAMESPACE
int debug = false;
//int debug = true;
END_NONAMESPACE

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ExImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ExImpl::ExImpl()
{
}

// @brief 値割り当てを１つ求める．
AssignList
ExImpl::get_assignment(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data{root, gvar_map, fvar_map, model};

  mData = &data;
  clear_queue();

  // 故障差の伝搬している経路を探す．
  ASSERT_COND( !mData->sensitized_output_list().empty() );
  auto spo = select_output();

  // その経路の side input の値を記録する．
  AssignList assign_list;
  put_queue(spo, 1);
  while ( !mQueue.empty() ) {
    auto node = get_queue();
    if ( node == root ) {
      continue;
    }
    int mark = mMarks.at(node->id());
    if ( debug ) {
      DBG_OUT << "visit at Node#" << node->id()
	      << ": " << mark << endl;
    }
    switch ( mark ) {
    case 1:
      // 故障の影響が伝搬しているノードの場合
      record_sensitized_node(node);
      break;
    case 2:
      // 故障の影響が伝搬していないノードの場合
      record_masking_node(node);
      break;
    case 3:
      { // Fanout-Cone の外側のノードの場合
	// 無条件に現在の値を記録する．
	bool val = (gval(node) == Val3::_1);
	assign_list.add(node, 1, val);
      }
      break;
    }
  }

  if ( debug ) {
    DBG_OUT << "Extract at ";
    const char* comma = "";
    DBG_OUT << comma << "Node#" << root->id() << endl;
    comma = "";
    for ( auto nv: assign_list ) {
      const TpgNode* node = nv.node();
      DBG_OUT << comma << "Node#" << node->id()
	      << ":";
      if ( nv.val() ) {
	DBG_OUT << "1";
      }
      else {
	DBG_OUT << "0";
      }
      comma = ", ";
    }
    DBG_OUT << endl;
  }

  return assign_list;
}

// @brief 故障の影響の伝搬を阻害する値割当を記録する．
void
ExImpl::record_sensitized_node(
  const TpgNode* node
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
    put_queue(inode, t);
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
ExImpl::record_masking_node(
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
    auto cnode = select_cnode(cnode_list);
    put_queue(cnode, 3);
    return;
  }

  // ここに来たということは全てのファンインに故障差が伝搬していないか
  // 複数のファンインの故障差が打ち消し合っているのですべてのファンイン
  // に再帰する．
  record_sensitized_node(node);
}

END_NAMESPACE_DRUID
