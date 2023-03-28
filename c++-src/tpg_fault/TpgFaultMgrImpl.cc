
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
  const TpgNetwork& network,
  FaultType fault_type,
  const string& red_mode
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

  case FaultType::GateExhaustive:
    obj = new TpgFaultMgr_Ex{network};
    break;

  case FaultType::None:
    ASSERT_NOT_REACHED;
    break;
  }

  obj->gen_all_faults(network, red_mode);

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

// @brief 故障リストを作る．
void
TpgFaultMgrImpl::gen_all_faults(
  const TpgNetwork& network,
  const string& red_mode
)
{
  extern void ffr_reduction(
    const TpgNetwork& network,
    const vector<TpgFaultImpl*>& fault_array,
    unordered_set<SizeType>& rep_map
  );

  // 故障の生成を行う．
  // 構造情報に基づいた代表故障を rep_map に入れる．
  unordered_set<SizeType> rep_map;
  _gen_all_faults(network, rep_map);

  if ( red_mode == "none" ) {
    // 全ての故障を代表故障にする．
    for ( auto f: mFaultArray ) {
      rep_map.emplace(f->id());
    }
  }
  else if ( red_mode == "struct" ) {
    // rep_map をそのまま使う．
    ;
  }
  else if ( red_mode == "ffr" ) {
    // FFR モードで代表故障を求める．
    ffr_reduction(network, mFaultArray, rep_map);
  }

  // rep_map に基づいて代表故障の設定を行う．
  for ( auto f: mFaultArray ) {
    SizeType fid = f->id();
    if ( rep_map.count(fid) > 0 ) {
      mRepFaultList.push_back(fid);
      auto node = f->origin_node();
      mNodeFaultList[node->id()].push_back(fid);
    }
  }

  for ( auto ffr: network.ffr_list() ) {
    auto& ffr_fault_list = mFFRFaultList[ffr.id()];
    for ( auto node: ffr.node_list() ) {
      for ( auto fid: mNodeFaultList[node->id()] ) {
	ffr_fault_list.push_back(fid);
      }
    }
  }

  for ( auto mffc: network.mffc_list() ) {
    auto& mffc_fault_list = mMFFCFaultList[mffc.id()];
    for ( auto ffr: mffc.ffr_list() ) {
      for ( auto fid: mFFRFaultList[ffr.id()] ) {
	mffc_fault_list.push_back(fid);
      }
    }
  }
}

// @brief 故障を登録する．
void
TpgFaultMgrImpl::reg_fault(
  TpgFaultImpl* fault
)
{
  SizeType id = mFaultArray.size();
  fault->set_id(id);
  mFaultArray.push_back(fault);
  mStatusArray.push_back(FaultStatus::Undetected);
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Struct
//////////////////////////////////////////////////////////////////////

// @brief 全ての故障を作る．
void
TpgFaultMgr_Struct::_gen_all_faults(
  const TpgNetwork& network,
  unordered_set<SizeType>& rep_map
)
{
  // 等価故障のルール
  // - ファンアウト数が1の場合の branch と stem
  // - 出力の controlling value の故障と入力の controlling value

  // PPI の出力の故障
  for ( auto node: network.ppi_list() ) {
    auto node_name = network.ppi_name(node->input_id());
    gen_ofault(node, node_name, rep_map);
  }

  // 論理ゲートの入出力の故障
  for ( auto gate: network.gate_list() ) {
    auto node_name = gate.name();
    // 出力の故障
    auto onode = gate.output_node();
    gen_ofault(onode, node_name, rep_map);
    // 入力の故障
    SizeType ni = gate.input_num();
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto binfo = gate.branch_info(i);
      auto inode = binfo.node;
      SizeType ipos = binfo.ipos;
      gen_ifault(gate, inode, node_name, ipos, rep_map);
    }
  }

  // PPO の入力の故障
  for ( auto node: network.ppo_list() ) {
    auto node_name = network.ppo_name(node->output_id());
    gen_ifault(node, node_name, rep_map);
  }
}

// @brief 出力の故障を作る．
void
TpgFaultMgr_Struct::gen_ofault(
  const TpgNode* node,
  const string& node_name,
  unordered_set<SizeType>& rep_map
)
{
  bool rep = node->fanout_num() >= 2;
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = new_ofault(node, node_name, fval);
    reg_fault(f);
    if ( rep ) {
      rep_map.emplace(f->id());
    }
  }
}

// @brief 入力の故障を作る．
void
TpgFaultMgr_Struct::gen_ifault(
  const TpgGate& gate,
  const TpgNode* node,
  const string& node_name,
  SizeType ipos,
  unordered_set<SizeType>& rep_map
)
{
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = new_ifault(node, node_name, ipos, fval);
    bool rep = gate.is_rep(ipos, fval);
    reg_fault(f);
    if ( rep ) {
      rep_map.emplace(f->id());
    }
  }
}

/// @brief 入力の故障を作る(PPO用)．
void
TpgFaultMgr_Struct::gen_ifault(
  const TpgNode* node,
  const string& node_name,
  unordered_set<SizeType>& rep_map
)
{
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = new_ifault(node, node_name, 0, fval);
    reg_fault(f);
    rep_map.emplace(f->id());
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

BEGIN_NONAMESPACE

// 遷移故障を表す文字列を作る．
inline
string
td_name(
  Fval2 fval
)
{
  switch ( fval ) {
  case Fval2::zero: return "RISE";
  case Fval2::one:  return "FALL";
  }
}

END_NONAMESPACE

// @brief 出力の故障を作る．
TpgFaultImpl*
TpgFaultMgr_Td::new_ofault(
  const TpgNode* node,
  const string& node_name,
  Fval2 fval
)
{
  ostringstream buf;
  buf << node_name << ":O:" << td_name(fval);
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
  buf << node_name << ":I" << ipos << ":" << td_name(fval);
  auto name = buf.str();
  auto f = new TpgFault_TdBranch{node, name, ipos, fval};
  return f;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr_Ex
//////////////////////////////////////////////////////////////////////

// @brief 全ての故障を作る．
void
TpgFaultMgr_Ex::_gen_all_faults(
  const TpgNetwork& network,
  unordered_set<SizeType>& rep_map
)
{
  // ゲート網羅故障には自明な等価故障はない．
  for ( auto gate: network.gate_list() ) {
    auto node_name = gate.name();
    auto onode = gate.output_node();
    SizeType ni = gate.input_num();
    if ( ni < 2 ) {
      continue;
    }
    SizeType ni_exp = 1 << ni;
    for ( SizeType b = 0; b < ni_exp; ++ b ) {
      vector<bool> ivals(ni, false);
      for ( SizeType i = 0; i < ni; ++ i ) {
	if ( b & (1 << i) ) {
	  ivals[i] = true;
	}
      }
      auto f = new_fault(onode, node_name, ivals);
      rep_map.emplace(f->id());
    }
  }
}

// @brief 故障のタイプを返す．
FaultType
TpgFaultMgr_Ex::fault_type() const
{
  return FaultType::GateExhaustive;
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
  reg_fault(f);
  return f;
}

END_NAMESPACE_DRUID
