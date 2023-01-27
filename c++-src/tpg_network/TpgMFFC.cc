
/// @file TpgMFFC.cc
/// @brief TpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgMFFC.h"
#include "TpgNetworkImpl.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

// @brief 根のノードを返す．
const TpgNode*
TpgMFFC::root() const
{
  return mNetwork->mffc_root(mId);
}

// @brief このMFFCに含まれるFFR番号のリストを返す．
const vector<TpgFFR>&
TpgMFFC::ffr_list() const
{
  return mNetwork->mffc_ffr_list(mId);
}

// @brief このFFRに含まれる代表故障のリストを返す．
const vector<const TpgFault*>&
TpgMFFC::fault_list() const
{
  return mNetwork->mffc_fault_list(mId);
}

END_NAMESPACE_DRUID
