
/// @file Fault_Ex.cc
/// @brief Fault_Ex の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Fault_Ex.h"
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

// @brief ゲート網羅故障を生成する．
FaultRep*
FaultRep::new_ex_fault(
  SizeType id,
  const GateRep* gate,
  const std::vector<bool>& ivals
)
{
  return new Fault_Ex{id, gate, ivals};
}


//////////////////////////////////////////////////////////////////////
// Fault_Ex
//////////////////////////////////////////////////////////////////////

// @brief 故障の種類を返す．
FaultType
Fault_Ex::fault_type() const
{
  return FaultType::GateExhaustive;
}

// @brief 故障値を返す．
Fval2
Fault_Ex::fval() const
{
  throw std::logic_error{"Not a stack-at/transition fault"};
}

// @brief 故障伝搬の起点となるノードを返す．
const NodeRep*
Fault_Ex::origin_node() const
{
  return gate()->output_node();
}

// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
std::vector<SizeType>
Fault_Ex::excitation_condition() const
{
  std::vector<SizeType> assign_list;
  auto ni = gate()->input_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    auto inode = gate()->input_node(i);
    assign_list.push_back(Assign::encode(inode->id(), 1, mIvals[i]));
  }
  return assign_list;
}

// @brief 故障の内容を表す文字列を返す．
std::string
Fault_Ex::str() const
{
  std::ostringstream buf;
  buf << gate()->name() << ":EX";
  auto ni = gate()->input_num();
  for ( SizeType i = 0; i < ni; ++ i ) {
    buf << mIvals[i];
  }
  return buf.str();
}

END_NAMESPACE_DRUID
