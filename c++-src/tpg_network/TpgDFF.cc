
/// @file TpgDFF.cc
/// @brief TpgDFF の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDFF.h"
#include "TpgDFFList.h"
#include "DFFImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDFF
//////////////////////////////////////////////////////////////////////

// @brief ID番号を返す．
SizeType
TpgDFF::id() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->id();
}

// @brief 入力端子のノードを返す．
const TpgNode*
TpgDFF::input() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->input();
}

// @brief 出力端子のノードを返す．
const TpgNode*
TpgDFF::output() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->output();
}

// @brief クリア端子のノードを返す．
const TpgNode*
TpgDFF::clear() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->clear();
}

// @brief プリセット端子のノードを返す．
const TpgNode*
TpgDFF::preset() const
{
  ASSERT_COND( mImpl != nullptr );

  return mImpl->preset();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgDFFIter
//////////////////////////////////////////////////////////////////////

// @brief dereference 演算子
TpgDFF
TpgDFFIter::operator*() const
{
  return TpgDFF{&(*mIter)};
}

// @brief next 演算子
TpgDFFIter&
TpgDFFIter::operator++()
{
  ++ mIter;
  return *this;
}

END_NAMESPACE_DRUID
