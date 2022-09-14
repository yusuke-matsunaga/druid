
/// @file TpgDff.cc
/// @brief TpgDff の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDff.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDff
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDff::TpgDff()
{
  mInput = nullptr;
  mOutput = nullptr;
  mClock = nullptr;
  mClear = nullptr;
  mPreset = nullptr;
}

// @brief デストラクタ
TpgDff::~TpgDff()
{
}

// @brief ID番号を返す．
SizeType
TpgDff::id() const
{
  return mId;
}

// @brief 入力端子のノードを返す．
const TpgNode*
TpgDff::input() const
{
  return mInput;
}

// @brief 出力端子のノードを返す．
const TpgNode*
TpgDff::output() const
{
  return mOutput;
}

// @brief クロック端子のノードを返す．
const TpgNode*
TpgDff::clock() const
{
  return mClock;
}

// @brief クリア端子のノードを返す．
const TpgNode*
TpgDff::clear() const
{
  return mClear;
}

// @brief プリセット端子のノードを返す．
const TpgNode*
TpgDff::preset() const
{
  return mPreset;
}

END_NAMESPACE_DRUID
