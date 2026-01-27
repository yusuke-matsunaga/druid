
/// @file TpgMFFC.cc
/// @brief TpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgMFFC.h"
#include "types/TpgFFR.h"
#include "types/TpgFFRList.h"
#include "types/TpgNode.h"
#include "MFFCRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgMFFC
//////////////////////////////////////////////////////////////////////

// @brief 根のノードを返す．
TpgNode
TpgMFFC::root() const
{
  return TpgBase::node(_mffc()->root());
}

// @brief このMFFCに含まれるFFR数を返す．
SizeType
TpgMFFC::ffr_num() const
{
  return _mffc()->ffr_num();
}

// @brief このMFFCに含まれるFFRを返す．
TpgFFR
TpgMFFC::ffr(
  SizeType index
) const
{
  return TpgBase::ffr(_mffc()->ffr(index));
}

// @brief このMFFCに含まれるFFRのリストを返す．
TpgFFRList
TpgMFFC::ffr_list() const
{
  return TpgBase::ffr_list(_mffc()->ffr_list());
}

END_NAMESPACE_DRUID
