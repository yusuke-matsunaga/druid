
/// @file SnInput.cc
/// @brief SnInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
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
  throw std::logic_error{"Not a GATE type"};
}

// @brief 内容をダンプする．
void
SnInput::dump(
  std::ostream& s
) const
{
  s << "INPUT"
    << std::endl;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnInput::_calc_val(
  const std::vector<FSIM_VALTYPE>& val_array
) const
{
  throw std::logic_error{"Not a GATE type"};
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnInput::_calc_gobs(
  const std::vector<FSIM_VALTYPE>& val_array,
  SizeType ipos
) const
{
  throw std::logic_error{"Not a GATE type"};
}

END_NAMESPACE_DRUID_FSIM
