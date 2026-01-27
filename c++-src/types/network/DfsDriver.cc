
/// @File DfsDriver.cc
/// @brief DfsDriver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "DfsDriver.h"
#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// DfsDriver
//////////////////////////////////////////////////////////////////////

// @brief dfs を行う．
void
DfsDriver::operator()(
  const NodeRep* node_rep
)
{
  if ( mMarkArray[node_rep->id()] ) {
    return;
  }
  mMarkArray[node_rep->id()] = true;

  mPreFunc(node_rep);

  for ( auto inode_rep: node_rep->fanin_list() ) {
    operator()(inode_rep);
  }

  mPostFunc(node_rep);
}

END_NAMESPACE_DRUID
