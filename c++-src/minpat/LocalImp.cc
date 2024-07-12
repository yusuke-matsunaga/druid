
/// @file LocalImp.cc
/// @brief LocalImp の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "LocalImp.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

// @brief 含意操作を行う．
NodeTimeValList
LocalImp::run(
  const NodeTimeValList& assignments
)
{
  mAssignList.clear();
  for ( SizeType i = 0; i < mNetwork.node_num(); ++ i ) {
    mValArray[0][i] = Val3::_X;
    mValArray[1][i] = Val3::_X;
  }
  mQueue.clear();
  for ( auto nv: assignments ) {
    assign(nv.node(), nv.time(), nv.val());
  }

  while ( !mQueue.empty() ) {
    auto nv = mQueue.front();
    mQueue.pop_front();
    auto node = nv.node();
    auto time = nv.time();
    auto val = nv.val() ? Val3::_1: Val3::_0;
    mValArray[time][node->id()] = val;
    // 前方含意
    forward_imp(node, time);
    // 後方含意
    backward_imp(node, time);
  }
  return mAssignList;
}

// @brief 前方含意を行う．
void
LocalImp::forward_imp(
  const TpgNode* node,
  int time
)
{
  auto val = mValArray[time][node->id()];
  auto bval = val == Val3::_1;
  if ( node->is_dff_input() && time == 0 ) {
    auto alt_node = node->alt_node();
    assign(alt_node, 1, bval);
    return;
  }
  for ( auto onode: node->fanout_list() ) {
    if ( get_val(onode, time) != Val3::_X ) {
      continue;
    }
    switch ( onode->gate_type() ) {
    case PrimType::None:
      break;
    case PrimType::C0:
      break;
    case PrimType::C1:
      break;
    case PrimType::Buff:
      assign(onode, time, bval);
      break;
    case PrimType::Not:
      assign(onode, time, !bval);
      break;
    case PrimType::And:
      if ( val == Val3::_0 ) {
	assign(onode, time, false);
      }
      else { // val == Val3::_1
	bool all_1 = true;
	for ( auto inode: onode->fanin_list() ) {
	  if ( get_val(inode, time) != Val3::_1 ) {
	    all_1 = false;
	    break;
	  }
	}
	if ( all_1 ) {
	  assign(onode, time, true);
	}
      }
      break;
    case PrimType::Nand:
      if ( val == Val3::_0 ) {
	assign(onode, time, true);
      }
      else { // val == Val3::_1
	bool all_1 = true;
	for ( auto inode: onode->fanin_list() ) {
	  if ( get_val(inode, time) != Val3::_1 ) {
	    all_1 = false;
	    break;
	  }
	}
	if ( all_1 ) {
	  assign(onode, time, false);
	}
      }
      break;
    case PrimType::Or:
      if ( val == Val3::_1 ) {
	assign(onode, time, true);
      }
      else { // val == Val3::_0
	bool all_0 = true;
	for ( auto inode: onode->fanin_list() ) {
	  if ( get_val(inode, time) != Val3::_0 ) {
	    all_0 = false;
	    break;
	  }
	}
	if ( all_0 ) {
	  assign(onode, time, false);
	}
      }
      break;
    case PrimType::Nor:
      if ( val == Val3::_1 ) {
	assign(onode, time, false);
      }
      else { // val == Val3::_0
	bool all_0 = true;
	for ( auto inode: onode->fanin_list() ) {
	  if ( get_val(inode, time) != Val3::_0 ) {
	    all_0 = false;
	    break;
	  }
	}
	if ( all_0 ) {
	  assign(onode, time, true);
	}
      }
      break;
    case PrimType::Xor:
      {
	bool oval = false;
	bool all_fixed = true;
	for ( auto inode: onode->fanin_list() ) {
	  auto ival = get_val(inode, time);
	  if ( ival == Val3::_X ) {
	    all_fixed = false;
	    break;
	  }
	  if ( ival == Val3::_1 ) {
	    oval = !oval;
	  }
	}
	if ( all_fixed ) {
	  assign(onode, time, oval);
	}
      }
      break;
    case PrimType::Xnor:
      {
	bool oval= false;
	bool all_fixed = true;
	for ( auto inode: onode->fanin_list() ) {
	  auto ival = get_val(inode, time);
	  if ( ival == Val3::_X ) {
	    all_fixed = false;
	    break;
	  }
	  if ( ival == Val3::_1 ) {
	    oval = !oval;
	  }
	}
	if ( all_fixed ) {
	  assign(onode, time, oval);
	}
      }
      break;
    }
  }
}

// @brief 後方含意を行う．
void
LocalImp::backward_imp(
  const TpgNode* node,
  int time
)
{
  auto val = get_val(node, time);
  auto bval = val == Val3::_1;
  if ( node->is_dff_output() && time == 1 ) {
    auto alt_node = node->alt_node();
    assign(alt_node, 0, bval);
    return;
  }
  switch ( node->gate_type() ) {
  case PrimType::None:
    break;
  case PrimType::C0:
    break;
  case PrimType::C1:
    break;
  case PrimType::Buff:
    assign(node->fanin(0), time, bval);
    break;
  case PrimType::Not:
    assign(node->fanin(0), time, !bval);
    break;
  case PrimType::And:
    if ( val == Val3::_1 ) {
      for ( auto inode: node->fanin_list() ) {
	assign(inode, time, true);
      }
    }
    else { // val == Val3::_0
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      bool has_0 = false;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_0 ) {
	  has_0 = true;
	  break;
	}
      }
      if ( !has_0 && xcount == 1 ) {
	assign(xnode, time, false);
      }
    }
    break;
  case PrimType::Nand:
    if ( val == Val3::_0 ) {
      for ( auto inode: node->fanin_list() ) {
	assign(inode, time, true);
      }
    }
    else { // val == Val3::_1
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      bool has_0 = false;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_0 ) {
	  has_0 = true;
	  break;
	}
      }
      if ( !has_0 && xcount == 1 ) {
	assign(xnode, time, false);
      }
    }
    break;
  case PrimType::Or:
    if ( val == Val3::_0 ) {
      for ( auto inode: node->fanin_list() ) {
	assign(inode, time, false);
      }
    }
    else { // val == Val3::_1
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      bool has_1 = false;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_1 ) {
	  has_1 = true;
	  break;
	}
      }
      if ( !has_1 && xcount == 1 ) {
	assign(xnode, time, true);
      }
    }
    break;
  case PrimType::Nor:
    if ( val == Val3::_1 ) {
      for ( auto inode: node->fanin_list() ) {
	assign(inode, time, false);
      }
    }
    else { // val == Val3::_0
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      bool has_1 = false;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_1 ) {
	  has_1 = true;
	  break;
	}
      }
      if ( !has_1 && xcount == 1 ) {
	assign(xnode, time, true);
      }
    }
    break;
  case PrimType::Xor:
    {
      bool bval = val == Val3::_1? true : false;
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_1 ) {
	  bval = !bval;
	}
      }
      if ( xcount == 1 ) {
	assign(xnode, time, bval);
      }
    }
    break;
  case PrimType::Xnor:
    {
      bool bval = val == Val3::_1? false : true;
      int xcount = 0;
      const TpgNode* xnode = nullptr;
      for ( auto inode: node->fanin_list() ) {
	auto ival = get_val(inode, time);
	if ( ival == Val3::_X ) {
	  xnode = inode;
	  ++ xcount;
	  if ( xcount >= 2 ) {
	    break;
	  }
	}
	else if ( ival == Val3::_1 ) {
	  bval = !bval;
	}
      }
      if ( xcount == 1 ) {
	assign(xnode, time, bval);
      }
    }
    break;
  }
}

// @brief ノードの値を読み出す．
Val3
LocalImp::get_val(
  const TpgNode* node,
  int time
)
{
  return mValArray[time][node->id()];
}

// @brief ノードに値を割り当てる．
void
LocalImp::assign(
  const TpgNode* node,
  int time,
  bool val
)
{
  if ( get_val(node, time) == Val3::_X ) {
    auto val3 = val ? Val3::_1 : Val3::_0;
    mValArray[time][node->id()] = val3;
    NodeTimeVal nv{node, time, val};
    mQueue.push_back(nv);
    mAssignList.add(nv);
  }
}

END_NAMESPACE_DRUID
