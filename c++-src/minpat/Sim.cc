
/// @file Sim.cc
/// @brief Sim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Sim.h"
#include "TpgNetwork.h"
#include "NodeTimeValList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

inline
PackedVal
calc_and(
  const vector<PackedVal>& ival_array
)
{
  auto val = PV_ALL1;
  for ( auto ival: ival_array ) {
    val &= ival;
  }
  return val;
}

inline
PackedVal
calc_or(
  const vector<PackedVal>& ival_array
)
{
  auto val = PV_ALL0;
  for ( auto ival: ival_array ) {
    val |= ival;
  }
  return val;
}

inline
PackedVal
calc_xor(
  const vector<PackedVal>& ival_array
)
{
  auto val = PV_ALL0;
  for ( auto ival: ival_array ) {
    val ^= ival;
  }
  return val;
}

inline
PackedVal
calc_val(
  PrimType gate_type,
  const vector<PackedVal>& ival_array
)
{
  switch ( gate_type ) {
  case PrimType::C0:   return PV_ALL0;
  case PrimType::C1:   return PV_ALL1;
  case PrimType::Buff: return ival_array[0];
  case PrimType::Not:  return ~ival_array[0];
  case PrimType::And:  return calc_and(ival_array);
  case PrimType::Nand: return ~calc_and(ival_array);
  case PrimType::Or:   return calc_or(ival_array);
  case PrimType::Nor:  return ~calc_or(ival_array);
  case PrimType::Xor:  return calc_xor(ival_array);
  case PrimType::Xnor: return ~calc_xor(ival_array);
  default:
    ASSERT_NOT_REACHED;
  }
  return PV_ALL0;
}

END_NONAMESPACE


// @brief コンストラクタ
Sim::Sim(
  const TpgNetwork& network
) : mNetwork{network},
    mValArray(mNetwork.node_num() * 2)
{
}

// @brief 乱数を用いたシミュレーションを行う．
void
Sim::sim_random()
{
  mPattern = TestVector{mNetwork};
  if ( mNetwork.has_prev_state() ) {
    for ( auto node: mNetwork.node_list() ) {
      calc_node_0(node);
    }
    for ( auto node: mNetwork.node_list() ) {
      calc_node_1(node);
    }
  }
  else {
    for ( auto node: mNetwork.node_list() ) {
      calc_node(node);
    }
  }
}

// @brief パタンを用いたシミュレーションを行う．
void
Sim::sim_pattern(
  const TestVector& pattern
)
{
  mPattern = pattern;
  if ( mNetwork.has_prev_state() ) {
    for ( auto node: mNetwork.node_list() ) {
      calc_node_0(node);
    }
    for ( auto node: mNetwork.node_list() ) {
      calc_node_1(node);
    }
  }
  else {
    for ( auto node: mNetwork.node_list() ) {
      calc_node(node);
    }
  }
}

// @brief 割り当て条件を満たしているか調べる．
PackedVal
Sim::check(
  const NodeTimeValList& assign
) const
{
  PackedVal dbits = PV_ALL1;
  for ( auto nv: assign ) {
    auto node = nv.node();
    auto time = nv.time();
    auto v = nv.val();
    auto val = node_val(node, time);
    if ( v ) {
      dbits &= val;
    }
    else {
      dbits &= ~val;
    }
  }
  return dbits;
}

// @brief ノードの値を計算する(組み合わせ回路用)．
void
Sim::calc_node(
  const TpgNode* node
)
{
  PackedVal val;
  if ( node->is_ppi() ) {
    auto b = mPattern.ppi_val(node->input_id());
    val = mRandDist(mRandGen);
    if ( b == Val3::_0 ) {
      val &= ~1UL;
    }
    else if ( b == Val3::_1 ) {
      val |= 1UL;
    }
  }
  else if ( node->is_logic() ) {
    SizeType ni = node->fanin_num();
    vector<PackedVal> ival_array;
    ival_array.reserve(ni);
    for ( auto inode: node->fanin_list() ) {
      ival_array.push_back(node_val(inode, 1));
    }
    val = calc_val(node->gate_type(), ival_array);
  }
  else if ( node->is_ppo() ) {
    auto inode = node->fanin(0);
    val = node_val(inode, 1);
  }
  set_node_val(node, 1, val);
}

// @brief ノードの値を計算する(順序回路の0時刻用)．
void
Sim::calc_node_0(
  const TpgNode* node
)
{
  PackedVal val;
  if ( node->is_ppi() ) {
    auto b = mPattern.input_val(node->input_id());
    val = mRandDist(mRandGen);
    if ( b == Val3::_0 ) {
      val &= ~1UL;
    }
    else if ( b == Val3::_1 ) {
      val |= 1UL;
    }
  }
  else if ( node->is_logic() ) {
    SizeType ni = node->fanin_num();
    vector<PackedVal> ival_array;
    ival_array.reserve(ni);
    for ( auto inode: node->fanin_list() ) {
      ival_array.push_back(node_val(inode, 0));
    }
    val = calc_val(node->gate_type(), ival_array);
  }
  else if ( node->is_ppo() ) {
    auto inode = node->fanin(0);
    val = node_val(inode, 0);
  }
  set_node_val(node, 0, val);
}

// @brief ノードの値を計算する(順序回路の1時刻用)．
void
Sim::calc_node_1(
  const TpgNode* node
)
{
  PackedVal val;
  if ( node->is_ppi() ) {
    if ( node->is_primary_input() ) {
      auto b = mPattern.aux_input_val(node->input_id());
      val = mRandDist(mRandGen);
      if ( b == Val3::_0 ) {
	val &= ~1UL;
      }
      else if ( b == Val3::_1 ) {
	val |= 1UL;
      }
    }
    else { // node->is_dff_output()
      auto alt_node = node->alt_node();
      val = node_val(alt_node, 0);
    }
  }
  else if ( node->is_logic() ) {
    SizeType ni = node->fanin_num();
    vector<PackedVal> ival_array;
    ival_array.reserve(ni);
    for ( auto inode: node->fanin_list() ) {
      ival_array.push_back(node_val(inode, 1));
    }
    val = calc_val(node->gate_type(), ival_array);
  }
  else if ( node->is_ppo() ) {
    auto inode = node->fanin(0);
    val = node_val(inode, 1);
  }
  set_node_val(node, 1, val);
}

END_NAMESPACE_DRUID
