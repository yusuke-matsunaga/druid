
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

// @brief 制御値を持つファンインを一つ選ぶ．
TpgNode
Just1::select_cval_node(
  const TpgNode& node,
  int time
)
{
  // cval を持つファンインを求める．
  auto cval = node.cval();
  TpgNodeList inode_list;
  for ( auto inode: node.fanin_list() ) {
    auto ival = just_data().val(inode, time);
    if ( ival == cval ) {
      inode_list.push_back(inode);
      if ( just_data().fixed_mark(inode, time) ) {
	return inode;
      }
    }
  }
  if ( inode_list.size() == 0 ) {
    throw std::logic_error{"never happend"};
  }
  return inode_list[0];
}

END_NAMESPACE_DRUID
