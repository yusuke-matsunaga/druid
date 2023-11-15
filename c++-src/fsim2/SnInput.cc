
/// @file SnInput.cc
/// @brief SnInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "SnInput.h"


BEGIN_NAMESPACE_DRUID_FSIM2

//////////////////////////////////////////////////////////////////////
// SnInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnInput::SnInput(
  SizeType id
) : SimNode{id}
{
  set_level(0);
}

// @brief デストラクタ
SnInput::~SnInput()
{
}

// @brief ゲートタイプを返す．
PrimType
SnInput::gate_type() const
{
  return PrimType::None;
}

// @brief ファンイン数を得る．
SizeType
SnInput::fanin_num() const
{
  return 0;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnInput::fanin(
  SizeType pos
) const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 内容をダンプする．
void
SnInput::dump(
  ostream& s
) const
{
  s << "INPUT" << endl;
}

// @brief 出力値の計算を行う．
FSIM2_VALTYPE
SnInput::_calc_val()
{
  ASSERT_NOT_REACHED;
  return FSIM2_VALTYPE(PV_ALL0);
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnInput::_calc_gobs(
  SizeType ipos
)
{
  ASSERT_NOT_REACHED;
  return PV_ALL0;
}

END_NAMESPACE_DRUID_FSIM2
