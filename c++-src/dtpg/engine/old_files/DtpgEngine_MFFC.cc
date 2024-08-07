﻿
/// @file DtpgEngine_MFFC.cc
/// @brief DtpgEngine_MFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine_MFFC.h"

#include "TpgFault.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateEnc.h"
#include "AssignList.h"
#include "TestVector.h"
#include "ym/Range.h"

//#define DEBUG_DTPG
#define DEBUG_OUT cout

BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_mffc = 1;
#else
const int debug_mffc = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgEngine_MFFC::DtpgEngine_MFFC(
  const TpgNetwork& network,
  const TpgMFFC* mffc,
  const JsonValue& option
) : DtpgEngine{network, mffc->root(), option},
    mMFFC{mffc},
    mRootArray(mffc->ffr_num()),
    mEvarArray(mffc->ffr_num())
{
  opt_make_cnf();
}

// @brief デストラクタ
DtpgEngine_MFFC::~DtpgEngine_MFFC()
{
}

// @brief make_cnf() の追加処理
void
DtpgEngine_MFFC::opt_make_cnf()
{
  SizeType ffr_id = 0;
  for ( auto ffr: mMFFC->ffr_list() ) {
    auto root = ffr->root();
    mRootArray[ffr_id] = root;
    mFFRIdMap.emplace(root->id(), ffr_id);

    auto cvar = new_variable(true);
    mEvarArray[ffr_id] = cvar;

    if ( debug_mffc ) {
      DEBUG_OUT << "cvar(FFR#" << ffr_id << ") = " << cvar << endl;
    }

    ++ ffr_id;
  }

  // mRootArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  auto node_list = mRootArray;
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    auto node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	auto var = new_variable(true);
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffc ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for ( SizeType i = 0; i < mRootArray.size(); ++ i ) {
    auto node = mRootArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    auto fvar = new_variable(true);
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  GateEnc fval_enc{solver(), fvar_map()};
  for ( auto node: node_list ) {
    auto ovar = fvar(node);
    if ( mFFRIdMap.count(node->id()) > 0 ) {
      SizeType ffr_pos = mFFRIdMap.at(node->id());
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mEvarArray[ffr_pos]
      ovar = new_variable();
      inject_fault(ffr_pos, ovar);
      // ovar が fvar(node) ではない！
      fval_enc.make_cnf(node, ovar);
    }
    else {
      fval_enc.make_cnf(node);
    }

    if ( debug_mffc ) {
      DEBUG_OUT << "Node#" << node->id() << ": ofvar("
		<< ovar << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << fvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 故障伝搬の起点ノードを返す．
const TpgNode*
DtpgEngine_MFFC::fault_origin(
  const TpgFault* fault
)
{
  return fault->ffr_root();
}

// @brief 故障の活性化条件
AssignList
DtpgEngine_MFFC::fault_condition(
  const TpgFault* fault
)
{
  return fault->ffr_propagate_condition();
}

// @brief gen_pattern() で用いる検出条件を作る．
vector<SatLiteral>
DtpgEngine_MFFC::extra_assumptions(
  const TpgFault* fault
)
{
  vector<SatLiteral> assumptions;
  auto ffr_root = fault->origin_node()->ffr_root();
  if ( ffr_root != root_node() ) {
    // ffr_root のある FFR を活性化する条件を作る．
    if ( mFFRIdMap.count(ffr_root->id()) == 0 ) {
      cerr << "Error[DtpgEngine_MFFC::dtpg()]: "
	   << ffr_root->id() << " is not within the MFFC" << endl;
      ASSERT_NOT_REACHED;
      return {};
    }

    SizeType ffr_id = mFFRIdMap.at(ffr_root->id());
    SizeType ffr_num = mRootArray.size();
    if ( ffr_num > 1 ) {
      // FFR の根の出力に故障を挿入する．
      assumptions.reserve(ffr_num);
      for ( auto i: Range(ffr_num) ) {
	auto evar = mEvarArray[i];
	bool inv = (i != ffr_id);
	auto lit = evar * inv;
	assumptions.push_back(lit);
      }
    }
  }

  return assumptions;
}

// @brief 故障挿入回路のCNFを作る．
void
DtpgEngine_MFFC::inject_fault(
  SizeType ffr_id,
  SatLiteral ovar
)
{
  auto lit1 = ovar;
  auto lit2 = mEvarArray[ffr_id];
  auto node = mRootArray[ffr_id];
  auto olit = fvar(node);

  solver().add_xorgate(lit1, lit2, olit);

  if ( debug_mffc ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << olit
	      << " with cvar = " << lit2 << endl;
  }
}

END_NAMESPACE_DRUID
