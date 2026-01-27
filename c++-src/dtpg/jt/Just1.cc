
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
  const TpgNetwork& network
) : JustBase{network}
{
}

// @brief デストラクタ
Just1::~Just1()
{
}

// @brief 初期化処理
void
Just1::just_init(
  const AssignList& assign_list
)
{
  // なにもしない．
}

// @brief 制御値を持つファンインを一つ選ぶ．
TpgNode
Just1::select_cval_node(
  const TpgNode& node,
  int time
)
{
  // cval を持つ最初のファンインをたどる．
  auto cval = node.cval();
  for ( auto inode: node.fanin_list() ) {
    auto ival = just_data().val(inode, time);
    if ( ival == cval ) {
      return inode;
    }
  }
  throw std::logic_error{"never happend"};
}

// @brief 終了処理
void
Just1::just_end()
{
  // なにもしない．
}

END_NAMESPACE_DRUID
