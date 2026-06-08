#ifndef PONODE_H
#define PONODE_H

/// @file POSet.h
/// @brief POSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class POSet;

//////////////////////////////////////////////////////////////////////
/// @class PONode PONode.h "PONode.h"
/// @brief POSet のノードを表すクラス
//////////////////////////////////////////////////////////////////////
class PONode
{
  friend class POSet;

public:

  /// @brief コンストラクタ
  PONode(
    SizeType id ///< [in] ID番号
  ) : mId{id}
  {
  }

  /// @brief デストラクタ
  ~PONode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す.
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 等価ノードの代表ノードを返す．
  const PONode*
  rep_node() const
  {
    return mRepNode;
  }

  /// @brief 後続ノードのリストを返す．
  const std::vector<PONode*>&
  succ_list() const
  {
    return mSuccList;
  }

  /// @brief 先行ノードのリストを返す．
  const std::vector<PONode*>&
  pred_list() const
  {
    return mPredList;
  }

  /// @brief 直接の後続ノードのリストを返す．
  const std::vector<PONode*>&
  imm_succ_list() const
  {
    return mImmSuccList;
  }

  /// @brief ランクを返す．
  SizeType
  rank() const
  {
    return mRank;
  }

  /// @grief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const
  {
    s << "Node#" << id();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  SizeType mId;

  // ランク
  SizeType mRank{0};

  bool mHasRank{false};

  SizeType mCount{0};

  // 等価ノードの代表ノード
  PONode* mRepNode{nullptr};

  // 後続ノードのリスト
  std::vector<PONode*> mSuccList;

  // 先行ノードのリスト
  std::vector<PONode*> mPredList;

  // 直接の後続ノードのリスト
  std::vector<PONode*> mImmSuccList;

};

END_NAMESPACE_DRUID

#endif // PONODE_H
