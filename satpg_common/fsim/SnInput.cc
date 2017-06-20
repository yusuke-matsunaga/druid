﻿
/// @file SnInput.cc
/// @brief SnInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "SnInput.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// SnInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnInput::SnInput(ymuint id) :
  SimNode(id)
{
  set_level(0);
}

// @brief デストラクタ
SnInput::~SnInput()
{
}

// @brief ゲートタイプを返す．
//
// ここでは kGateBUFF を返す．
GateType
SnInput::gate_type() const
{
  return kGateINPUT;
}

// @brief ファンイン数を得る．
ymuint
SnInput::fanin_num() const
{
  return 0;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnInput::fanin(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 内容をダンプする．
void
SnInput::dump(ostream& s) const
{
  s << "INPUT" << endl;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnInput::_calc_val()
{
  ASSERT_NOT_REACHED;
  return FSIM_VALTYPE(kPvAll0);
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnInput::_calc_gobs(ymuint ipos)
{
  ASSERT_NOT_REACHED;
  return kPvAll0;
}

END_NAMESPACE_YM_SATPG_FSIM
