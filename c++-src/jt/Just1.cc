
/// @file Just1.cc
/// @brief Just1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
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
Just1::Just1(
  SizeType max_id
) : JustImpl{max_id}
{
}

// @brief デストラクタ
Just1::~Just1()
{
}

// @brief 初期化処理
void
Just1::just_init(
  const NodeValList& assign_list,
  const JustData& jd
)
{
  // なにもしない．
}

// @brief 制御値を持つファンインを一つ選ぶ．
const TpgNode*
Just1::select_cval_node(
  const JustData& jd,
  const TpgNode* node,
  int time
)
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
