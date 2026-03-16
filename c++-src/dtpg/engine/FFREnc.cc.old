
/// @file FFREnc.cc
/// @brief FFREnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/FFREnc.h"
#include "types/TpgNodeList.h"
#include "types/TpgFFR.h"
#include "types/TpgFault.h"
#include "types/Val3.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREnc::FFREnc(
  SatLiteral root_pvar,
  const TpgFFR& ffr,
  const TpgFaultList& fault_list
) : mRootPropVar{root_pvar},
    mFFR{ffr},
    mFaultList{fault_list},
    mNodeList{mFFR.node_list()}
{
  for ( auto f: mFaultList ) {
    auto ex_cond = f.excitation_condition();
    for ( auto nv: ex_cond ) {
      if ( nv.time() == 0 ) {
	auto node = nv.node();
	mPrevNodeList.push_back(node);
      }
    }
  }
}

// @brief データ構造の初期化を行う．
void
FFREnc::init()
{
}

// @brief 必要な変数を割り当てCNF式を作る．
void
FFREnc::make_cnf()
{
  auto root = mFFR.root();
  // 根から先の伝搬条件
  if ( mRootPropVar != SatLiteral::X ) {
    mPropNodeVarMap.emplace(root.id(), mRootPropVar);
  }
  else {
    auto pvar = new_variable(true);
    solver().add_clause(pvar);
    mPropNodeVarMap.emplace(root.id(), pvar);
  }

  // DFS の in-order でノードごとの伝搬条件を作る．
  if ( !root.is_ppi() ) {
    make_cnf_sub(root);
  }

  // 故障の伝搬条件を作る．
  for ( auto fault: mFaultList ) {
    auto pvar = new_variable(true);
    auto ex_cond = fault.excitation_condition();
    auto tmp_lits = conv_to_literal_list(ex_cond);
    auto node = fault.origin_node();
    auto nvar = mPropNodeVarMap.at(node.id());
    tmp_lits.push_back(nvar);
    solver().add_andgate(pvar, tmp_lits);
    mPropFaultVarMap.emplace(fault.id(), pvar);
  }
}

void
FFREnc::make_cnf_sub(
  const TpgNode& node
)
{
  // node の伝搬条件を表す変数
  auto ovar = mPropNodeVarMap.at(node.id());
  auto nval = node.nval();
  if ( node.fanin_num() == 1 || node.nval() == Val3::_X ) {
    // ovar をそのままファンインの条件とする．
    for ( auto inode: node.fanin_list() ) {
      mPropNodeVarMap.emplace(inode.id(), ovar);
    }
  }
  else {
    // side-input の値を nval にしたものを伝搬条件に加える．
    auto bval = nval == Val3::_1;
    SizeType ni = node.fanin_num();
    std::vector<SatLiteral> lit_array(ni);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto inode = node.fanin(i);
      auto nv = Assign{inode, 1, bval};
      auto lit = conv_to_literal(nv);
      lit_array[i] = lit;
    }
    // 2乗のループを避けるため前半と後半の配列を作る．
    std::vector<std::vector<SatLiteral>> tmp_list1(ni);
    std::vector<std::vector<SatLiteral>> tmp_list2(ni);
    for ( SizeType i = 1; i < ni; ++ i ) {
      tmp_list1[i] = tmp_list1[i - 1];
      auto lit = lit_array[i - 1];
      tmp_list1[i].push_back(lit);
    }
    for ( SizeType i = 1; i < ni; ++ i ) {
      tmp_list2[ni - i - 1] = tmp_list2[ni - i];
      auto lit = lit_array[ni - i];
      tmp_list2[ni - i - 1].push_back(lit);
    }
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto inode = node.fanin(i);
      std::vector<SatLiteral> cond;
      cond.reserve(ni);
      cond.push_back(ovar);
      for ( auto lit: tmp_list1[i] ) {
	cond.push_back(lit);
      }
      for ( auto lit: tmp_list2[i] ) {
	cond.push_back(lit);
      }
      auto plit = new_variable(true);
      solver().add_andgate(plit, cond);
      mPropNodeVarMap.emplace(inode.id(), plit);
    }
  }
  for ( auto inode: node.fanin_list() ) {
    if ( inode.ffr_root() == inode || inode.is_ppi() ) {
      continue;
    }
    make_cnf_sub(inode);
  }
}

// @brief 故障伝搬条件を表す変数を返す．
SatLiteral
FFREnc::prop_var(
  const TpgFault& fault
)
{
  return mPropFaultVarMap.at(fault.id());
}

// @brief 関連するノードのリストを返す．
const TpgNodeList&
FFREnc::node_list() const
{
  return mNodeList;
}

// @brief 1時刻前の値に関連するノードのリストを返す．
const TpgNodeList&
FFREnc::prev_node_list() const
{
  return mPrevNodeList;
}

END_NAMESPACE_DRUID
