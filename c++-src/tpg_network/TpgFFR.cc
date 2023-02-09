
/// @file TpgFFR.cc
/// @brief TpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFFR.h"
#include "TpgNetworkImpl.h"
#include "FFRImpl.h"


BEGIN_NAMESPACE_DRUID

// @brief 根のノードを返す．
const TpgNode*
TpgFFR::root() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.root();
}

// @brief 葉(FFRの入力)のリストを返す．
const vector<const TpgNode*>&
TpgFFR::input_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.input_list();
}

// @brief このFFRに含まれる代表故障のリストを返す．
const vector<const TpgFault*>&
TpgFFR::fault_list() const
{
  ASSERT_COND( mNetwork != nullptr );

  auto& ffr = mNetwork->_ffr(mId);
  return ffr.fault_list();
}

END_NAMESPACE_DRUID
