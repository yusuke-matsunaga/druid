
/// @file TpgFaultMgr.cc
/// @brief TpgFaultMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultMgrImpl.h"
#include "TpgFaultMgr_Struct.h"
#include "TpgFaultMgr_Ex.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFaultImpl.h"
#include "Fval2.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgrImpl
//////////////////////////////////////////////////////////////////////

// @brief 派生クラスを生成するクラスメソッド
unique_ptr<TpgFaultMgrImpl>
TpgFaultMgrImpl::new_obj(
  const TpgNetwork& network, ///< [in] 対象のネットワーク
  FaultType fault_type       ///< [in] 故障の種類
)
{
  TpgFaultMgrImpl* obj = nullptr;
  switch ( fault_type ) {
  case FaultType::StuckAt:
    obj = new TpgFaultMgr_Sa{network};
    break;

  case FaultType::TransitionDelay:
    obj = new TpgFaultMgr_Td{network};
    break;

  case FaultType::GateExaustive:
    obj = new TpgFaultMgr_Ex{network};
    break;

  case FaultType::None:
    ASSERT_NOT_REACHED;
    break;
  }

  obj->gen_all_faults(network);

  for ( auto ffr: network.ffr_list() ) {
    auto& ffr_fault_list = obj->mFFRFaultList[ffr.id()];
    for ( auto node: ffr.node_list() ) {
      for ( auto f: obj->mNodeFaultList[node->id()] ) {
	ffr_fault_list.push_back(f);
      }
    }
  }

  for ( auto mffc: network.mffc_list() ) {
    auto& mffc_fault_list = obj->mMFFCFaultList[mffc.id()];
    for ( auto ffr: mffc.ffr_list() ) {
      for ( auto f: obj->mFFRFaultList[ffr.id()] ) {
	mffc_fault_list.push_back(f);
      }
    }
  }

  return unique_ptr<TpgFaultMgrImpl>{obj};
}

// @brief コンストラクタ
TpgFaultMgrImpl::TpgFaultMgrImpl(
  const TpgNetwork& network
) : mNodeFaultList(network.node_num()),
    mFFRFaultList(network.ffr_num()),
    mMFFCFaultList(network.mffc_num())
{
}

// @brief デストラクタ
TpgFaultMgrImpl::~TpgFaultMgrImpl()
{
  for ( auto fault: mFaultArray ) {
    delete fault;
  }
}

// @brief 故障を登録する．
void
TpgFaultMgrImpl::reg_fault(
  TpgFaultImpl* fault,
  bool rep
)
{
  SizeType id = mFaultArray.size();
  fault->set_id(id);
  mFaultArray.push_back(fault);
  mStatusArray.push_back(FaultStatus::Undetected);
  if ( rep ) {
    mRepFaultList.push_back(id);
    auto node = fault->origin_node();
    mNodeFaultList[node->id()].push_back(id);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Struct
//////////////////////////////////////////////////////////////////////

// @brief 全ての故障を作る．
void
TpgFaultMgr_Struct::gen_all_faults(
  const TpgNetwork& network
)
{
  // 等価故障のルール
  // - ファンアウト数が1の場合の branch と stem
  // - 出力の controlling value の故障と入力の controlling value

  // PPI の出力の故障
  for ( auto node: network.ppi_list() ) {
    auto node_name = network.ppi_name(node->input_id());
    if ( node->is_datapath() ) {
      gen_ofault(node, node_name);
    }
  }

  // 論理ゲートの入出力の故障
  for ( auto gate: network.gate_list() ) {
    auto node_name = gate.name();
    // 出力の故障
    auto onode = gate.output_node();
    if ( !onode->is_datapath() ) {
      continue;
    }
    gen_ofault(onode, node_name);
    // 入力の故障
    SizeType ni = gate.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto binfo = gate.branch_info(i);
      auto inode = binfo.node;
      SizeType ipos = binfo.ipos;
      auto oval0 = gate.cval(ipos, Val3::_0);
      auto oval1 = gate.cval(ipos, Val3::_1);
      gen_ifault(inode, node_name, ipos, oval0, oval1);
    }
  }

  // PPO の入力の故障
  for ( auto node: network.ppo_list() ) {
    auto node_name = network.ppo_name(node->output_id());
    gen_ifault(node, node_name, 0, Val3::_X, Val3::_X);
  }
}

// @brief 出力の故障を作る．
void
TpgFaultMgr_Struct::gen_ofault(
  const TpgNode* node,
  const string& node_name
)
{
  bool rep = node->fanout_num() >= 2;
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = new_ofault(node, node_name, fval);
    reg_fault(f, rep);
  }
}

/// @brief 入力の故障を作る．
void
TpgFaultMgr_Struct::gen_ifault(
  const TpgNode* node,
  const string& node_name,
  SizeType ipos,
  Val3 oval0,
  Val3 oval1
)
{
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = new_ifault(node, node_name, ipos, fval);
    bool rep = false;
    if ( node->is_ppo() ) {
      rep = true;
    }
    else {
      if ( fval == Fval2::zero ) {
	if ( oval0 == Val3::_X ) {
	  rep = true;
	}
      }
      else {
	if ( oval1 == Val3::_X ) {
	  rep = true;
	}
      }
    }
    reg_fault(f, rep);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Sa
//////////////////////////////////////////////////////////////////////

// @brief 故障のタイプを返す．
FaultType
TpgFaultMgr_Sa::fault_type() const
{
  return FaultType::StuckAt;
}

// @brief 出力の故障を作る．
TpgFaultImpl*
TpgFaultMgr_Sa::new_ofault(
  const TpgNode* node,
  const string& node_name,
  Fval2 fval
)
{
  ostringstream buf;
  buf << node_name << ":O:SA" << fval;
  auto name = buf.str();
  auto f = new TpgFault_SaStem{node, name, fval};
  return f;
}

// @brief 入力の故障を作る．
TpgFaultImpl*
TpgFaultMgr_Sa::new_ifault(
  const TpgNode* node,
  const string& node_name,
  SizeType ipos,
  Fval2 fval
)
{
  ostringstream buf;
  buf << node_name << ":I" << ipos << ":SA" << fval;
  auto name = buf.str();
  auto f = new TpgFault_SaBranch{node, name, ipos, fval};
  return f;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Td
//////////////////////////////////////////////////////////////////////

// @brief 故障のタイプを返す．
FaultType
TpgFaultMgr_Td::fault_type() const
{
  return FaultType::TransitionDelay;
}

// @brief 出力の故障を作る．
TpgFaultImpl*
TpgFaultMgr_Td::new_ofault(
  const TpgNode* node,
  const string& node_name,
  Fval2 fval
)
{
  ostringstream buf;
  buf << node_name << ":O:TD" << fval;
  auto name = buf.str();
  auto f = new TpgFault_TdStem{node, name, fval};
  return f;
}

// @brief 入力の故障を作る．
TpgFaultImpl*
TpgFaultMgr_Td::new_ifault(
  const TpgNode* node,
  const string& node_name,
  SizeType ipos,
  Fval2 fval
)
{
  ostringstream buf;
  buf << node_name << ":I" << ipos << ":TD" << fval;
  auto name = buf.str();
  auto f = new TpgFault_TdBranch{node, name, ipos, fval};
  return f;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Ex
//////////////////////////////////////////////////////////////////////

// @brief 全ての故障を作る．
void
TpgFaultMgr_Ex::gen_all_faults(
  const TpgNetwork& network
)
{
  // ゲート網羅故障には自明な等価故障はない．
  for ( auto gate: network.gate_list() ) {
    auto node_name = gate.name();
    auto onode = gate.output_node();
    if ( !onode->is_datapath() ) {
      continue;
    }
    SizeType ni = gate.input_num();
    SizeType ni_exp = 1 << ni;
    for ( SizeType b = 0; b < ni_exp; ++ b ) {
      vector<bool> ivals(ni, 0);
      for ( SizeType i = 0; i < ni; ++ i ) {
	if ( b & (1 << i) ) {
	  ivals[i] = true;
	}
	new_fault(onode, node_name, ivals);
      }
    }
  }
}

// @brief 故障のタイプを返す．
FaultType
TpgFaultMgr_Ex::fault_type() const
{
  return FaultType::GateExaustive;
}

// @brief 故障を作る．
TpgFaultImpl*
TpgFaultMgr_Ex::new_fault(
  const TpgNode* node,
  const string& node_name,
  const vector<bool>& ivals
)
{
  ostringstream buf;
  buf << node_name << ":EX";
  for ( auto b: ivals ) {
    buf << b;
  }
  auto name = buf.str();
  auto f = new TpgFault_Ex{node, name, ivals};
  reg_fault(f, true);
  return f;
}

END_NAMESPACE_DRUID
