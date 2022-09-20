#ifndef NODEMAP_H
#define NODEMAP_H

/// @file NodeMap.h
/// @brief NodeMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeMap NodeMap.h "NodeMap.h"
/// @brief BnNode と TpgNode の対応を記録するクラス
//////////////////////////////////////////////////////////////////////
class NodeMap
{
public:

  /// @brief コンストラクタ
  NodeMap() = default;

  /// @brief デストラクタ
  ~NodeMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録する．
  void
  reg(
    SizeType bnnode_id, ///< [in] BnNode のID番号
    TpgNode* tpgnode    ///< [in] TpgNode
  )
  {
    ASSERT_COND( mNodeMap.count(bnnode_id) == 0 );
    mNodeMap.emplace(bnnode_id, tpgnode);
  }

  /// @brief 対応するノードを得る．
  TpgNode*
  get(
    SizeType bnnode_id ///< [in] BnNode のID番号
  ) const
  {
    if ( mNodeMap.count(bnnode_id) > 0 ) {
      return mNodeMap.at(bnnode_id);
    }
    return nullptr;
}


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BnNode::id() をキーにした配列
  unordered_map<int, TpgNode*> mNodeMap;

};

END_NAMESPACE_DRUID

#endif // NODEMAP_H
