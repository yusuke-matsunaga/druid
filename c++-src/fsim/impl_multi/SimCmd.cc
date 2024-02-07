
/// @file SimCmd.cc
/// @brief SimCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimCmd.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimCmd
//////////////////////////////////////////////////////////////////////

// @brief タイプを返す．
SimCmd::Type
SimCmd::type() const
{
  return END;
}

/// @brief ノードを返す．
const SimNode*
SimCmd::node() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 反転マスクを返す．
Packedval
SimCmd::mask() const
{
  ASSERT_NOT_REACHED;
  return PV_ALL0;
}

// @brief ノードリストを返す．
const vector<const SimNode*>&
SimCmd::node_list() const
{
  ASSERT_NOT_REACHED;
  static vector<const SimNode*> dummy;
  return dummy;
}


//////////////////////////////////////////////////////////////////////
// クラス SimCmd1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimCmd1::SimCmd1(
  SizeType id,
  const SimNode* node,
  PackedVal mask
) : SimCmd{id},
    mNode{node},
    mMask{mask}
{
}

// @brief デストラクタ
SimCmd1::~SimCmd1()
{
}

/// @brief タイプを返す．
SimCmd::Type
SimCmd1::type() const
{
  return PPSFP;
}

// @brief ノードを返す．
const SimNode*
SimCmd1::node() const
{
  return mNode;
}

// @brief 反転マスクを返す．
Packedval
SimCmd1::mask() const
{
  return mMask;
}


//////////////////////////////////////////////////////////////////////
// クラス SimCmd2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimCmd2::SimCmd2(
  SizeType id,
  const vector<const SimNode*>& node_list
) : SimCmd{id},
    mNodeList{node_list}
{
}

// @brief デストラクタ
SimCmd2::~SimCmd2()
{
}

// @brief タイプを返す．
SimCmd::Type
SimCmd2::type() const
{
  return SPPFP;
}

const vector<const SimNode*>&
SimCmd2::node_list() const
{
  return mNodeList;
}

END_NAMESPACE_DRUID_FSIM
