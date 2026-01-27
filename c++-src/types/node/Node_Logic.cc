
/// @file Node_Logic.cc
/// @brief Node_Logic の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Node_Logic.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NodeRep
//////////////////////////////////////////////////////////////////////

// @brief 論理ノードを作る．
NodeRep*
NodeRep::new_logic(
  SizeType id,
  PrimType gate_type,
  const std::vector<const NodeRep*>& fanin_list
)
{
  auto ni = fanin_list.size();
  switch ( gate_type ) {
  case PrimType::C0:
    if ( ni != 0 ) {
      throw std::logic_error{"ni != 0"};
    }
    return new Node_C0(id);

  case PrimType::C1:
    if ( ni != 0 ) {
      throw std::logic_error{"ni != 0"};
    }
    return new Node_C1(id);

  case PrimType::Buff:
    if ( ni != 1 ) {
      throw std::logic_error{"ni != 1"};
    }
    return new Node_BUFF(id, fanin_list[0]);

  case PrimType::Not:
    if ( ni != 1 ) {
      throw std::logic_error{"ni != 1"};
    }
    return new Node_NOT(id, fanin_list[0]);

  case PrimType::And:
    return new Node_AND(id, fanin_list);

  case PrimType::Nand:
    return new Node_NAND(id, fanin_list);

  case PrimType::Or:
    return new Node_OR(id, fanin_list);

  case PrimType::Nor:
    return new Node_NOR(id, fanin_list);

  case PrimType::Xor:
    return new Node_XOR(id, fanin_list);

  case PrimType::Xnor:
    return new Node_XNOR(id, fanin_list);

  default:
    break;
  }

  throw std::logic_error{"never be reached"};
}


//////////////////////////////////////////////////////////////////////
// クラス Node_Logic
//////////////////////////////////////////////////////////////////////

// @brief logic タイプの時 true を返す．
bool
Node_Logic::is_logic() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC0
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_C0::gate_type() const
{
  return PrimType::C0;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_C1
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_C1::gate_type() const
{
  return PrimType::C1;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_BUFF
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_BUFF::gate_type() const
{
  return PrimType::Buff;
}

// @brief controling value を得る．
Val3
Node_BUFF::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
Node_BUFF::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
Node_BUFF::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
Node_BUFF::noval() const
{
  return Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_NOT
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_NOT::gate_type() const
{
  return PrimType::Not;
}

// @brief controling value を得る．
Val3
Node_NOT::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
Node_NOT::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
Node_NOT::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
Node_NOT::noval() const
{
  return Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_AND
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_AND::gate_type() const
{
  return PrimType::And;
}

// @brief controling value を得る．
Val3
Node_AND::cval() const
{
  return Val3::_0;
}

// @brief noncontroling valueを得る．
Val3
Node_AND::nval() const
{
  return Val3::_1;
}

// @brief controling output value を得る．
Val3
Node_AND::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
Val3
Node_AND::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_NAND
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_NAND::gate_type() const
{
  return PrimType::Nand;
}

// @brief controling output value を得る．
Val3
Node_NAND::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
Val3
Node_NAND::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_OR
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_OR::gate_type() const
{
  return PrimType::Or;
}

// @brief controling value を得る．
Val3
Node_OR::cval() const
{
  return Val3::_1;
}

// @brief noncontroling valueを得る．
Val3
Node_OR::nval() const
{
  return Val3::_0;
}

// @brief controling output value を得る．
Val3
Node_OR::coval() const
{
  return Val3::_1;
}

// @brief noncontroling output value を得る．
Val3
Node_OR::noval() const
{
  return Val3::_0;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_NOR
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_NOR::gate_type() const
{
  return PrimType::Nor;
}

// @brief controling output value を得る．
Val3
Node_NOR::coval() const
{
  return Val3::_0;
}

// @brief noncontroling output value を得る．
Val3
Node_NOR::noval() const
{
  return Val3::_1;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_XOR
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_XOR::gate_type() const
{
  return PrimType::Xor;
}

// @brief controling value を得る．
Val3
Node_XOR::cval() const
{
  return Val3::_X;
}

// @brief noncontroling valueを得る．
Val3
Node_XOR::nval() const
{
  return Val3::_X;
}

// @brief controling output value を得る．
Val3
Node_XOR::coval() const
{
  return Val3::_X;
}

// @brief noncontroling output value を得る．
Val3
Node_XOR::noval() const
{
  return Val3::_X;
}


//////////////////////////////////////////////////////////////////////
// クラス Node_XNOR
//////////////////////////////////////////////////////////////////////

// @brief ゲートタイプを得る．
PrimType
Node_XNOR::gate_type() const
{
  return PrimType::Xnor;
}

END_NAMESPACE_DRUID
