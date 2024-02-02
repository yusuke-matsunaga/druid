
/// @file SnInput.cc
/// @brief SnInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "SnInput.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// SnInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnInput::SnInput(
  SizeType id
) : SimNode{id, 0}
{
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
SizeType
SnInput::fanin(
  SizeType pos
) const
{
  ASSERT_NOT_REACHED;
  return 0;
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
FSIM_VALTYPE
SnInput::_calc_val(
  const vector<FSIM_VALTYPE>& val_array
) const
{
  ASSERT_NOT_REACHED;
  return FSIM_VALTYPE(PV_ALL0);
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnInput::_calc_gobs(
  const vector<FSIM_VALTYPE>& val_array,
  SizeType ipos
) const
{
  ASSERT_NOT_REACHED;
  return PV_ALL0;
}

END_NAMESPACE_DRUID_FSIM
