
/// @file TpgDffControl.cc
/// @brief TpgDffControl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDffControl.h"
#include "TpgDffClock.h"
#include "TpgDffClear.h"
#include "TpgDffPreset.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgDffControl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffControl::TpgDffControl(
  TpgDFF dff,
  const TpgNode* fanin
) : TpgNode{{fanin}},
    mDff{dff}
{
}

#if 0
// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
TpgDFF
TpgDffControl::dff() const
{
  return mDff;
}
#endif

//////////////////////////////////////////////////////////////////////
// クラス TpgDffClock
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffClock::TpgDffClock(
  TpgDFF dff,
  const TpgNode* fanin
) : TpgDffControl{dff, fanin}
{
}

// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
bool
TpgDffClock::is_dff_clock() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgDffClear
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffClear::TpgDffClear(
  TpgDFF dff,
  const TpgNode* fanin
) : TpgDffControl{dff, fanin}
{
}

// @brief DFF のクリア端子に接続している力タイプの時 true を返す．
bool
TpgDffClear::is_dff_clear() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgDffPreset
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDffPreset::TpgDffPreset(
  TpgDFF dff,
  const TpgNode* fanin
) : TpgDffControl{dff, fanin}
{
}

// @brief DFF のプリセット端子に接続している力タイプの時 true を返す．
bool
TpgDffPreset::is_dff_preset() const
{
  return true;
}

END_NAMESPACE_DRUID
