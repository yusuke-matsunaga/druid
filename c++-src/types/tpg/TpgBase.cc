
/// @File TpgBase.cc
/// @brief TpgBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "types/TpgBase.h"
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "types/TpgGate.h"
#include "types/TpgGateList.h"
#include "types/TpgFFR.h"
#include "types/TpgFFRList.h"
#include "types/TpgMFFC.h"
#include "types/TpgMFFCList.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"
#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateRep.h"
#include "MFFCRep.h"
#include "FFRRep.h"
#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// TpgBase
//////////////////////////////////////////////////////////////////////

// @brief ノード番号を TpgNode に変換する．
TpgNode
TpgBase::node(
  SizeType node_id
) const
{
  return TpgNode(_network(), node_id);
}

// @brief ノード番号のリストを TpgNodeList に変換する．
TpgNodeList
TpgBase::node_list(
  const std::vector<SizeType>& node_id_list
) const
{
  return TpgNodeList(_network(), node_id_list);
}

// @brief NodeRep* を TpgNode に変換する．
TpgNode
TpgBase::node(
  const NodeRep* node_rep
) const
{
  if ( node_rep == nullptr ) {
    return TpgNode{};
  }
  return node(node_rep->id());
}

// @brief NodeRep* のリストを TpgNode のリストに変換する．
TpgNodeList
TpgBase::node_list(
  const std::vector<const NodeRep*>& node_rep_list
) const
{
  std::vector<SizeType> id_list;
  id_list.reserve(node_rep_list.size());
  for ( auto node_rep: node_rep_list ) {
    id_list.push_back(node_rep->id());
  }
  return node_list(id_list);
}

// @brief ゲート番号を TpgGate に変換する．
TpgGate
TpgBase::gate(
  SizeType gate_id
) const
{
  return TpgGate(_network(), gate_id);
}

// @brief ゲート番号のリストを TpgGateList に変換する．
TpgGateList
TpgBase::gate_list(
  const std::vector<SizeType>& gate_id_list
) const
{
  return TpgGateList(_network(), gate_id_list);
}

// @brief GateRep* を TpgGate に変換する．
TpgGate
TpgBase::gate(
  const GateRep* gate_rep
) const
{
  if ( gate_rep == nullptr ) {
    return TpgGate{};
  }
  return gate(gate_rep->id());
}

// @brief GateRep* のリストを TpgGateList に変換する．
TpgGateList
TpgBase::gate_list(
  const std::vector<const GateRep*>& gate_rep_list
) const
{
  std::vector<SizeType> id_list;
  id_list.reserve(gate_rep_list.size());
  for ( auto gate_rep: gate_rep_list ) {
    id_list.push_back(gate_rep->id());
  }
  return gate_list(id_list);
}

// @brief FFR番号を TpgFFR に変換する．
TpgFFR
TpgBase::ffr(
  SizeType ffr_id
) const
{
  return TpgFFR(_network(), ffr_id);
}

// @brief FFR番号のリストを TpgFFRList に変換する．
TpgFFRList
TpgBase::ffr_list(
  const std::vector<SizeType>& ffr_id_list
) const
{
  return TpgFFRList(_network(), ffr_id_list);
}

// @brief FFRRep* を TpgFFR に変換する．
TpgFFR
TpgBase::ffr(
  const FFRRep* ffr_rep
) const
{
  if ( ffr_rep == nullptr ) {
    return TpgFFR{};
  }
  return ffr(ffr_rep->id());
}

// @brief FFRRep* のリストを TpgFFRList に変換する．
TpgFFRList
TpgBase::ffr_list(
  const std::vector<const FFRRep*>& ffr_rep_list
) const
{
  std::vector<SizeType> id_list;
  id_list.reserve(ffr_rep_list.size());
  for ( auto ffr_rep: ffr_rep_list ) {
    id_list.push_back(ffr_rep->id());
  }
  return ffr_list(id_list);
}

// @brief MFFC番号を TpgMFFC に変換する．
TpgMFFC
TpgBase::mffc(
  SizeType mffc_id
) const
{
  return TpgMFFC(_network(), mffc_id);
}

// @brief MFFC番号のリストを TpgMFFCList に変換する．
TpgMFFCList
TpgBase::mffc_list(
  const std::vector<SizeType>& mffc_id_list
) const
{
  return TpgMFFCList(_network(), mffc_id_list);
}

// @brief MFFCRep* を TpgMFFC に変換する．
TpgMFFC
TpgBase::mffc(
  const MFFCRep* mffc_rep
) const
{
  if ( mffc_rep == nullptr ) {
    return TpgMFFC{};
  }
  return mffc(mffc_rep->id());
}

// @brief MFFRRep* のリストを TpgMFFCList に変換する．
TpgMFFCList
TpgBase::mffc_list(
  const std::vector<const MFFCRep*>& mffc_rep_list
) const
{
  std::vector<SizeType> id_list;
  id_list.reserve(mffc_rep_list.size());
  for ( auto mffc_rep: mffc_rep_list ) {
    id_list.push_back(mffc_rep->id());
  }
  return mffc_list(id_list);
}

// @brief 故障番号を TpgFault に変換する．
TpgFault
TpgBase::fault(
  SizeType fault_id
) const
{
  return TpgFault(_network(), fault_id);
}

// @brief 故障番号のリストを TpgFaultList に変換する．
TpgFaultList
TpgBase::fault_list(
  const std::vector<SizeType>& fault_id_list
) const
{
  return TpgFaultList(_network(), fault_id_list);
}

// @brief FaultRep* を TpgFault に変換する．
TpgFault
TpgBase::fault(
  const FaultRep* fault_rep
) const
{
  if ( fault_rep == nullptr ) {
    return TpgFault{};
  }
  return fault(fault_rep->id());
}

// @brief FaultRep* のリストを TpgFaultList に変換する．
TpgFaultList
TpgBase::fault_list(
  const std::vector<const FaultRep*>& fault_rep_list
) const
{
  std::vector<SizeType> id_list;
  id_list.reserve(fault_rep_list.size());
  for ( auto fault_rep: fault_rep_list ) {
    id_list.push_back(fault_rep->id());
  }
  return fault_list(id_list);
}

// @brief NodeRep のポインタを取り出す．
const NodeRep*
TpgBase::_node(
  SizeType node_id
) const
{
  _check_valid();
  return _network()->node(node_id);
}

// @brief NodeRep のポインタを取り出す．
const NodeRep*
TpgBase::_node(
  const TpgNode& node
) const
{
  if ( node.is_valid() ) {
    return _node(node.id());
  }
  return nullptr;
}

// @brief NodeRep のポインタのリストに変換する．
std::vector<const NodeRep*>
TpgBase::_node_list(
  const std::vector<SizeType>& id_list
) const
{
  std::vector<const NodeRep*> node_rep_list;
  node_rep_list.reserve(id_list.size());
  for ( auto id: id_list ) {
    node_rep_list.push_back(_node(id));
  }
  return node_rep_list;
}

// @brief GateRep のポインタを取り出す．
const GateRep*
TpgBase::_gate(
  SizeType gid
) const
{
  _check_valid();
  return _network()->gate(gid);
}

// @brief FFRRep のポインタを取り出す．
const FFRRep*
TpgBase::_ffr(
  SizeType ffr_id
) const
{
  _check_valid();
  return _network()->ffr(ffr_id);
}

// @brief MFFCRep のポインタを取り出す．
const MFFCRep*
TpgBase::_mffc(
  SizeType mffc_id
) const
{
  _check_valid();
  return _network()->mffc(mffc_id);
}

// @brief FaultRep のポインタを取り出す．
const FaultRep*
TpgBase::_fault(
  SizeType fid
) const
{
  _check_valid();
  return _network()->fault(fid);
}

END_NAMESPACE_DRUID
