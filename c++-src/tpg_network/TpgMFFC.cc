
/// @file TpgMFFC.cc
/// @brief TpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgMFFC.h"
#include "TpgNetworkImpl.h"
#include "TpgFFR.h"
#include "MFFCImpl.h"


BEGIN_NAMESPACE_DRUID

// @brief 根のノードを返す．
const TpgNode*
TpgMFFC::root() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& mffc = mNetwork->_mffc(mId);
  return mffc.root();
}

// @brief このMFFCに含まれるFFR番号のリストを返す．
const vector<TpgFFR>&
TpgMFFC::ffr_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& mffc = mNetwork->_mffc(mId);
  return mffc.ffr_list();
}

// @brief このFFRに含まれる代表故障のリストを返す．
const vector<const TpgFault*>&
TpgMFFC::fault_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& mffc = mNetwork->_mffc(mId);
  return mffc.fault_list();
}

END_NAMESPACE_DRUID
