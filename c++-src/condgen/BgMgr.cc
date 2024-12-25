
/// @file BgMgr.cc
/// @brief BgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BgMgr.h"
#include "BgNode.h"
#include "ym/BddVar.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
BgMgr::BgMgr(
  const vector<Bdd>& bdd_list
)
{
  for ( auto bdd: bdd_list ) {
    auto root = convert(bdd);
    mRootList.push_back(root);
  }
}

// @brief デストラクタ
BgMgr::~BgMgr()
{
  for ( auto node: mNodeList ) {
    delete node;
  }
}

// @brief BDD から変換する．
const BgNode*
BgMgr::convert(
  const Bdd& bdd
)
{
  if ( bdd.is_zero() ) {
    return nullptr;
  }
  else if ( bdd.is_one() ) {
    return nullptr;
  }

  // 結果が登録されていないか調べる．
  if ( mNodeDict.count(bdd) > 0 ) {
    return mNodeDict.at(bdd);
  }

  // 根のノードで分解する．
  const BgNode* node = nullptr;
  Bdd bdd0;
  Bdd bdd1;
  auto root_var = bdd.root_decomp(bdd0, bdd1);
  auto varid = root_var.id();

  // 場合分け
  if ( bdd0.is_zero() ) {
    if ( bdd1.is_one() ) {
      // case-PLIT
      node = new_PLIT(varid);
    }
    else {
      // case-AND1
      node = new_AND(bdd);
    }
  }
  else if ( bdd0.is_one() ) {
    if ( bdd1.is_zero() ) {
      // case-NLIT
      node = new_NLIT(varid);
    }
    else {
      // case-OR0
      node = new_OR(bdd);
    }
  }
  else if ( bdd1.is_zero() ) {
    // case-AND0
    node = new_AND(bdd);
  }
  else if ( bdd.is_one() ) {
    // case-OR1
    node = new_OR(bdd);
  }
  else {
    // case-DEC
    auto cof0 = convert(bdd0);
    auto cof1 = convert(bdd1);
    node = new_DEC(varid, cof0, cof1);
  }

  mNodeDict.emplace(bdd, node);
  mNodeList.push_back(node);

  return node;
}

// @brief PLIT ノードを作る．
const BgNode*
BgMgr::new_PLIT(
  SizeType varid
)
{
  return BgNode::new_PLIT(varid);
}

// @brief NLIT ノードを作る．
const BgNode*
BgMgr::new_NLIT(
  SizeType varid
)
{
  return BgNode::new_NLIT(varid);
}

// @brief AND ノードを作る．
const BgNode*
BgMgr::new_AND(
  const Bdd& bdd
)
{
  vector<const BgNode*> operand_list;
  auto cofactor = search_AND(bdd, operand_list);
  return BgNode::new_AND(operand_list, cofactor);
}

// @brief OR ノードを作る．
const BgNode*
BgMgr::new_OR(
  const Bdd& bdd
)
{
  vector<const BgNode*> operand_list;
  auto cofactor = search_OR(bdd, operand_list);
  return BgNode::new_OR(operand_list, cofactor);
}

// @brief DEC ノードを作る．
const BgNode*
BgMgr::new_DEC(
  SizeType varid,
  const BgNode* cofactor0,
  const BgNode* cofactor1
)
{
  return BgNode::new_DEC2(varid, cofactor0, cofactor1);
}

// @brief AND ノードの境界を求める．
const BgNode*
BgMgr::search_AND(
  Bdd bdd,
  vector<const BgNode*>& operand_list
)
{
  while ( true ) {
    Bdd bdd0;
    Bdd bdd1;
    auto root_var = bdd.root_decomp(bdd0, bdd1);
    auto varid = root_var.id();
    if ( bdd0.is_zero() ) {
      auto node = new_PLIT(varid);
      operand_list.push_back(node);
      if ( bdd1.is_one() ) {
	return nullptr;
      }
      bdd = bdd1;
    }
    else if ( bdd1.is_zero() ) {
      auto node = new_NLIT(varid);
      operand_list.push_back(node);
      if ( bdd0.is_one() ) {
	return nullptr;
      }
      bdd = bdd0;
    }
    else {
      // bdd0/bdd1 のどちらも 0 ではなかった．
      return convert(bdd);
    }
  }
}

// @brief OR ノードの境界を求める．
const BgNode*
BgMgr::search_OR(
  Bdd bdd,
  vector<const BgNode*>& operand_list
)
{
  while ( true ) {
    Bdd bdd0;
    Bdd bdd1;
    auto root_var = bdd.root_decomp(bdd0, bdd1);
    auto varid = root_var.id();
    if ( bdd0.is_one() ) {
      auto node = new_NLIT(varid);
      operand_list.push_back(node);
      if ( bdd1.is_zero() ) {
	return nullptr;
      }
      bdd = bdd1;
    }
    else if ( bdd1.is_one() ) {
      auto node = new_PLIT(varid);
      operand_list.push_back(node);
      if ( bdd0.is_zero() ) {
	return nullptr;
      }
      bdd = bdd0;
    }
    else {
      // bdd0/bdd1 のどちらも 0 ではなかった．
      return convert(bdd);
    }
  }
}

END_NAMESPACE_DRUID
