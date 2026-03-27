
/// @file MkBdd.cc
/// @brief MkBdd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "MkBdd.h"
#include "PropGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス MkBdd
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MkBdd::MkBdd(
  const PropGraph& data,
  const std::vector<TpgNode>& node_list,
  const std::vector<TpgNode>& aux_side_inputs
) : mData{data}
{
  for ( auto& node: node_list ) {
    if ( data.gval(node) == Val3::_1 ) {
      mBddDict.emplace(node.id(), Bdd::one());
    }
    else {
      mBddDict.emplace(node.id(), Bdd::zero());
    }
  }
  auto root = data.root();
  auto root_id = root.id();
  auto root_var = mMgr.variable_bdd(0);
  mBddDict.emplace(root_id, root_var);
  auto n = aux_side_inputs.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& node = aux_side_inputs[i];
    auto var = mMgr.variable_bdd(i + 1);
    mBddDict.emplace(node.id(), var);
  }
  mNextVar = n + 1;
}

// @brief ノードの論理関数を表すBDDを作る．
Bdd
MkBdd::make_bdd(
  const TpgNode& node
)
{
  auto node_id = node.id();
  if ( mBddDict.count(node_id) > 0 ) {
    return mBddDict.at(node_id);
  }

  Bdd bdd;
  if ( mData.type(node) == 3 ) {
    // 新しい変数を割り当てる．
    bdd = mMgr.variable_bdd(mNextVar);
    ++ mNextVar;
  }
  else {
    auto ni = node.fanin_num();
    switch ( node.gate_type() ) {
    case PrimType::None:
      break;
    case PrimType::C0:
      bdd = Bdd::zero();
      break;
    case PrimType::C1:
      bdd = Bdd::one();
      break;
    case PrimType::Buff:
      bdd = make_bdd(node.fanin(0));
      break;
    case PrimType::Not:
      bdd = ~make_bdd(node.fanin(0));
      break;
    case PrimType::And:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd &= make_bdd(inode);
      }
      break;
    case PrimType::Nand:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd &= make_bdd(inode);
      }
      bdd = ~bdd;
      break;
    case PrimType::Or:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd |= make_bdd(inode);
      }
      break;
    case PrimType::Nor:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd |= make_bdd(inode);
      }
      bdd = ~bdd;
      break;
    case PrimType::Xor:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd ^= make_bdd(inode);
      }
      break;
    case PrimType::Xnor:
      bdd = Bdd::one();
      for ( SizeType i = 0; i < ni; ++ i ) {
	auto inode = node.fanin(i);
	bdd ^= make_bdd(inode);
      }
      bdd = ~bdd;
      break;
    }
  }
  mBddDict.emplace(node.id(), bdd);
  return bdd;
}

// @brief 起点のノードの変数
BddVar
MkBdd::root_var()
{
  return mMgr.variable(0);
}

// @brief aux_side_input の変数
BddVar
MkBdd::aux_side_input_var(
  SizeType pos
)
{
  return mMgr.variable(pos + 1);
}

END_NAMESPACE_DRUID
