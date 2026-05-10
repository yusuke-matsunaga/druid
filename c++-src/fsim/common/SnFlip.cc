
/// @file SnFlip.cc
/// @brief SnFlip の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnFlip.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SnFlip
//////////////////////////////////////////////////////////////////////

// @brief デストラクタ
SnFlip::~SnFlip()
{
}

// @brief ゲートタイプを返す．
PrimType
SnFlip::gate_type() const
{
  return PrimType::Buff;
}

// @brief 内容をダンプする．
void
SnFlip::dump(
  std::ostream& s
) const
{
  s << "FLIP("
    << _fanin()->id()
    << ")"
    << std::endl;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnFlip::_calc_val() const
{
  auto val = _fanin()->calc_val();
  return val ^ mFlipMask;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnFlip::_calc_gobs(
  SizeType ipos
) const
{
  return PV_ALL1;
}

END_NAMESPACE_DRUID_FSIM
