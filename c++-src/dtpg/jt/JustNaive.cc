
/// @file JustNaive.cc
/// @brief JustNaive の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "JustNaive.h"
#include "JustData.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug_jt = 0;

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス JustNaive
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
JustNaive::JustNaive(
  const TpgNetwork& network
) : JustImpl{network}
{
}

// @brief デストラクタ
JustNaive::~JustNaive()
{
}

// @brief justify の実際の処理
AssignList
JustNaive::_justify(
  const AssignList& assign_list ///< [in] 割当リスト
)
{
  // 時刻1の割り当てを持つノードのリスト
  vector<const TpgNode*> cur_list;
  // 時刻0の割り当てを持つノードのリスト
  vector<const TpgNode*> prev_list;
  for ( auto nv: assign_list ) {
    auto node = nv.node();
    if ( nv.time() == 1 ) {
      cur_list.push_back(node);
    }
    else {
      prev_list.push_back(node);
    }
  }

  AssignList pi_assign_list;

  TpgNodeSet::get_tfi_list(
    network().node_num(), cur_list,
    [&](const TpgNode* node) {
      if ( debug_jt ) {
	cout << "Node#" << node->id()
	     << "@1: " << get_bval(node, 1) << endl;
      }
      if ( node->is_ppi() ) {
	bool bval = get_bval(node, 1);
	if ( has_prev_state() ) {
	  if ( node->is_primary_input() ) {
	    pi_assign_list.add(node, 1, bval);
	  }
	  else {
	    prev_list.push_back(node->alt_node());
	  }
	}
	else {
	  pi_assign_list.add(node, 1, bval);
	}
      }
    });

  if ( has_prev_state() ) {
    TpgNodeSet::get_tfi_list(
      network().node_num(), prev_list,
      [&](const TpgNode* node) {
	if ( debug_jt ) {
	  cout << "Node#" << node->id()
	       << "@0: " << get_bval(node, 0) << endl;
	}
	if ( node->is_ppi() ) {
	  bool bval = get_bval(node, 0);
	  pi_assign_list.add(node, 0, bval);
	}
      });
  }

  return pi_assign_list;
}

// @brief 値を取り出す．
bool
JustNaive::get_bval(
  const TpgNode* node,
  int time
)
{
  return just_data().val(node, time) == Val3::_1;
}

END_NAMESPACE_DRUID
