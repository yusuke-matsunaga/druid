
/// @file Fault_Stem.cc
/// @brief Fault_Stem の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Fault_Stem.h"
#include "GateRep.h"
#include "NodeRep.h"
#include "types/FaultType.h"
#include "types/TpgNode.h"
#include "types/Assign.h"
#include "types/Fval2.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FaultRep
//////////////////////////////////////////////////////////////////////

// @brief ステムの故障を生成する．
FaultRep*
FaultRep::new_stem_fault(
  SizeType id,
  const GateRep* gate,
  Fval2 fval,
  FaultType fault_type
)
{
  switch ( fault_type ) {
  case FaultType::StuckAt:
    if ( fval == Fval2::zero ) {
      return new Fault_StemSa0{id, gate};
    }
    else {
      return new Fault_StemSa1{id, gate};
    }
  case FaultType::TransitionDelay:
    if ( fval == Fval2::zero ) {
      return new Fault_StemRise{id, gate};
    }
    else {
      return new Fault_StemFall{id, gate};
    }
  default:
    throw std::logic_error{"never be reached"};
  }
  // ダミー
  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// Fault_Stem
//////////////////////////////////////////////////////////////////////

// @brief 故障伝搬の起点となるノードを返す．
const NodeRep*
Fault_Stem::origin_node() const
{
  return gate()->output_node();
}

// @brief 故障の内容を表す文字列の基本部分を返す．
std::string
Fault_Stem::str_base() const
{
  return gate()->name() + ":O";
}


//////////////////////////////////////////////////////////////////////
// Fault_StemSa
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
Fault_StemSa::fault_type() const
{
  return FaultType::StuckAt;
}


//////////////////////////////////////////////////////////////////////
// Fault_StemSa0
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_StemSa0::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_StemSa0::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 0 縮退故障なので 1 にする．
  assign_list.push_back(Assign::encode(origin_node()->id(), 1, true));
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_StemSa0::str() const
{
  return str_base() + ":SA0";
}


//////////////////////////////////////////////////////////////////////
// Fault_StemSa1
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_StemSa1::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_StemSa1::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  // 1 縮退故障なので 0 にする．
  assign_list.push_back(Assign::encode(origin_node()->id(), 1, false));
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_StemSa1::str() const
{
  return str_base() + ":SA1";
}


//////////////////////////////////////////////////////////////////////
// Fault_StemTd
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
Fault_StemTd::fault_type() const
{
  return FaultType::TransitionDelay;
}


//////////////////////////////////////////////////////////////////////
// Fault_StemRise
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_StemRise::fval() const
{
  return Fval2::zero;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_StemRise::excitation_condition() const
{
  // 0 -> 1 の遷移を起こす．
  std::vector<SizeType> assign_list;
  assign_list.push_back(Assign::encode(origin_node()->id(), 0, false));
  assign_list.push_back(Assign::encode(origin_node()->id(), 1, true));
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_StemRise::str() const
{
  return str_base() + ":RISE";
}


//////////////////////////////////////////////////////////////////////
// Fault_StemFall
//////////////////////////////////////////////////////////////////////

// @brief 故障値を得る．
Fval2
Fault_StemFall::fval() const
{
  return Fval2::one;
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_StemFall::excitation_condition() const
{
  // 1 -> 0 の遷移を起こす．
  std::vector<SizeType> assign_list;
  assign_list.push_back(Assign::encode(origin_node()->id(), 0, true));
  assign_list.push_back(Assign::encode(origin_node()->id(), 1, false));
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_StemFall::str() const
{
  return str_base() + ":FALL";
}

END_NAMESPACE_DRUID
