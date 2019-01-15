
/// @file Just1.cc
/// @brief Just1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Just1.h"
#include "JustData.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

int debug = 0;

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス Just1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
Just1::Just1(int max_id) :
  JustImpl(max_id)
{
}

// @brief デストラクタ
Just1::~Just1()
{
}

// @brief 初期化処理
// @param[in] assign_list 割当リスト
// @param[in] jd justify 用のデータ
void
Just1::just_init(const NodeValList& assign_list,
		 const JustData& jd)
{
  // なにもしない．
}

// @brief 制御値を持つファンインを一つ選ぶ．
// @param[in] jd justiry用のデータ
// @param[in] node 対象のノード
// @param[in] time 時刻 ( 0 or 1 )
// @return 選んだファンインのノードを返す．
const TpgNode*
Just1::select_cval_node(const JustData& jd,
			const TpgNode* node,
			int time)
{
  // cval を持つ最初のファンインをたどる．
  Val3 cval = node->cval();
  for ( auto inode: node->fanin_list() ) {
    Val3 ival = jd.val(inode, time);
    if ( ival == cval ) {
      return inode;
    }
  }
  ASSERT_NOT_REACHED;

  return nullptr;
}

// @brief 終了処理
void
Just1::just_end()
{
  // なにもしない．
}

END_NAMESPACE_DRUID
