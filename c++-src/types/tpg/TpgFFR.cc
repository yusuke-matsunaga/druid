
/// @file TpgFFR.cc
/// @brief TpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgFFR.h"
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "FFRRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFFR
//////////////////////////////////////////////////////////////////////

// @brief 根のノードを返す．
TpgNode
TpgFFR::root() const
{
  return TpgBase::node(_ffr()->root());
}

// @brief 葉(FFRの入力)の数を返す．
SizeType
TpgFFR::input_num() const
{
  return _ffr()->input_num();
}

// @brief 葉(FFRの入力)を返す．
TpgNode
TpgFFR::input(
  SizeType index
) const
{
  return TpgBase::node(_ffr()->input(index));
}

// @brief 葉(FFRの入力)のリストを返す．
TpgNodeList
TpgFFR::input_list() const
{
  return TpgBase::node_list(_ffr()->input_list());
}

// @brief このFFRに含まれるノード数を返す．
SizeType
TpgFFR::node_num() const
{
  return _ffr()->node_num();
}

// @brief このFFRに含まれるノードを返す．
TpgNode
TpgFFR::node(
  SizeType index
) const
{
  return TpgBase::node(_ffr()->node(index));
}

// @brief このFFRに含まれるノードのリストを返す．
TpgNodeList
TpgFFR::node_list() const
{
  return TpgBase::node_list(_ffr()->node_list());
}

END_NAMESPACE_DRUID
