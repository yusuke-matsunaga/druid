
/// @file NodeMap.cc
/// @brief NodeMap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "NodeMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeMap
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
NodeMap::NodeMap()
{
}

// @brief デストラクタ
NodeMap::~NodeMap()
{
}

// @brief 登録する．
// @param[in] bnnode_id BnNodeのID番号
// @param[in] tpgnode TpgNode
void
NodeMap::reg(int bnnode_id,
	     TpgNode* tpgnode)
{
  ASSERT_COND( !mNodeMap.check(bnnode_id) );
  mNodeMap.add(bnnode_id, tpgnode);
}

// @brief 対応するノードを得る．
TpgNode*
NodeMap::get(int bnnode_id) const
{
  TpgNode* node;
  if ( mNodeMap.find(bnnode_id, node) ) {
    return node;
  }
  return nullptr;
}

END_NAMESPACE_DRUID
