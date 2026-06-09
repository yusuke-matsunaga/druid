
/// @file POSet.cc
/// @brief POSet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "POSet.h"
#include "PONode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス POSet
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
POSet::POSet()
{
}

// @brief コンストラクタ
POSet::POSet(
  const Builder& builder
)
{
  _set(builder);
}

// @brief デストラクタ
POSet::~POSet()
{
}

// @brief 最初期化
void
POSet::rebuild(
  const Builder& builder
)
{
  mNodeList.clear();
  mRankArray.clear();
  _set(builder);
}

// @brief 内容を設定する．
void
POSet::_set(
  const Builder& builder
)
{
  auto size = builder.mSize;
  mNodeList.reserve(size);
  for ( SizeType id = 0; id < size; ++ id ) {
    auto node = new PONode(id);
    mNodeList.push_back(std::unique_ptr<PONode>{node});
  }

  // 支配関係を表すリンクを作る．
  for ( auto& p: builder.mElemList ) {
    auto id1 = p.first;
    auto id2 = p.second;
    auto node1 = mNodeList[id1].get();
    auto node2 = mNodeList[id2].get();
    node1->mSuccList.push_back(node2);
    node2->mPredList.push_back(node1);
    ++ node2->mCount;
  }

  // ランクを計算する．
  std::vector<PONode*> node_list;
  for ( auto& node: mNodeList ) {
    if ( node->mCount == 0 ) {
      node_list.push_back(node.get());
    }
  }
  for ( SizeType rank = 0; !node_list.empty(); ++ rank ) {
    for ( auto node: node_list ) {
      node->mRank = rank;
      node->mHasRank = true;
    }
    std::vector<PONode*> new_list;
    for ( auto node: node_list ) {
      for ( auto node1: node->mSuccList ) {
	-- node1->mCount;
	if ( node1->mCount == 0 ) {
	  new_list.push_back(node1);
	}
      }
    }
    std::swap(node_list, new_list);
  }
  // mImmSuccList を作る．
  for ( auto& node: mNodeList ) {
    auto ref_rank = node->rank() + 1;
    for ( auto node1: node->mSuccList ) {
      auto rank1 = node1->rank();
      if ( rank1 == ref_rank ) {
	node->mImmSuccList.push_back(node1);
      }
    }
  }
  // mRankArray を作る．
  SizeType max_rank = 0;
  for ( auto& node: mNodeList ) {
    max_rank = std::max(max_rank, node->rank());
  }
  ++ max_rank;
  mRankArray.resize(max_rank);
  std::vector<SizeType> count_array(max_rank, 0);
  for ( auto& node: mNodeList ) {
    auto rank = node->rank();
    ++ count_array[rank];
  }
  for ( SizeType rank = 0; rank < max_rank; ++ rank ) {
    mRankArray[rank].reserve(count_array[rank]);
  }
  for ( auto& node: mNodeList ) {
    auto rank = node->rank();
    mRankArray[rank].push_back(node->id());
  }
}

// @brief 後続の要素番号のリストを返す．
std::vector<SizeType>
POSet::succ_list(
  SizeType id
) const
{
  auto node = _node(id);
  return _id_list(node->mSuccList);

}

// @brief 先行の要素番号のリストを返す．
std::vector<SizeType>
POSet::pred_list(
  SizeType id
) const
{
  auto node = _node(id);
  return _id_list(node->mPredList);
}

// @brief 直接の後続の要素番号のリストを返す．
std::vector<SizeType>
POSet::imm_succ_list(
  SizeType id
) const
{
  auto node = _node(id);
  return _id_list(node->mImmSuccList);
}

// @brief ノードのリストを要素番号のリストに変換する．
std::vector<SizeType>
POSet::_id_list(
  const std::vector<PONode*>& node_list
) const
{
  std::vector<SizeType> ans_list;
  ans_list.reserve(node_list.size());
  for ( auto node: node_list ) {
    ans_list.push_back(node->id());
  }
  return ans_list;
}

// @brief 内容を出力する．
void
POSet::print(
  std::ostream& s
) const
{
  for ( SizeType rank = 0; rank < rank_size(); ++ rank ) {
    s << "Rank#" << rank << ":" << std::endl;
    for ( auto id: mRankArray[rank] ) {
      s << id << "  --> ";
      auto& node = mNodeList[id];
      for ( auto node1: node->mImmSuccList ) {
	s << " " << node1->id();
      }
      s << std::endl;
    }
    s << std::endl;
  }
}

END_NAMESPACE_DRUID
