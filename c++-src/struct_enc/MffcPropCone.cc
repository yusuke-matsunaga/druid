
/// @file MffcPropCone.cc
/// @brief MffcPropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MffcPropCone.h"
#include "StructEnc.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "GateEnc.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

BEGIN_NONAMESPACE

bool debug_mffccone = false;

#define DEBUG_OUT cout

END_NONAMESPACE

// @brief コンストラクタ
MffcPropCone::MffcPropCone(
  StructEnc& struct_sat,
  const TpgMFFC& mffc,
  const TpgNode* block_node,
  bool detect
) : PropCone{struct_sat, mffc.root(), block_node, detect},
    mElemArray(mffc.ffr_num()),
    mElemVarArray(mffc.ffr_num())
{
  for ( SizeType i = 0; i < mffc.ffr_num(); ++ i ) {
    auto ffr = mffc.ffr(i);
    mElemArray[i] = ffr->root();
    ASSERT_COND( ffr->root() != nullptr );
    mElemPosMap.emplace(ffr->root()->id(), i);
  }
}

// @brief デストラクタ
MffcPropCone::~MffcPropCone()
{
}

// @brief 関係するノードの変数を作る．
void
MffcPropCone::make_vars()
{
  PropCone::make_vars();
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
MffcPropCone::make_cnf()
{
  PropCone::make_cnf();

  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for ( SizeType i = 0; i < mElemArray.size(); ++ i ) {
    mElemVarArray[i] = solver().new_variable();

    if ( debug_mffccone ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  unordered_map<int, int> ffr_map;
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    auto node = mElemArray[i];
    ffr_map.emplace(node->id(), i);
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	auto var = solver().new_variable(true);
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    auto node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	auto var = solver().new_variable(true);
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for ( SizeType i = 0; i < mElemArray.size(); ++ i ) {
    auto node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    auto fvar = solver().new_variable(true);
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  GateEnc gate_enc(solver(), fvar_map());
  for ( SizeType rpos = 0; rpos < node_list.size(); ++ rpos ) {
    auto node = node_list[rpos];
    auto ovar = fvar(node);
    int ffr_pos;
    if ( ffr_map.count(node->id()) > 0 ) {
      auto ffr_pos = ffr_map.at(node->id());
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[ffr_pos]
      ovar = solver().new_variable();
      inject_fault(ffr_pos, ovar);
      // ovar が fvar(node) ではない！
      gate_enc.make_cnf(node, ovar);
    }
    else {
      gate_enc.make_cnf(node);
    }

    if ( debug_mffccone ) {
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

// @brief 故障挿入回路のCNFを作る．
void
MffcPropCone::inject_fault(
  int ffr_pos,
  SatLiteral ovar
)
{
  auto lit1 = ovar;
  auto lit2 = mElemVarArray[ffr_pos];
  auto node = mElemArray[ffr_pos];
  auto olit = fvar(node);

  solver().add_xorgate(lit1, lit2, olit);

  if ( debug_mffccone ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[ffr_pos] << endl;
  }
}

// @brief 故障の影響伝搬させる条件を作る．
vector<SatLiteral>
MffcPropCone::make_condition(
  const TpgNode* root
)
{
  // root のある FFR を活性化する条件を作る．
  if  ( mElemPosMap.count(root->id()) == 0 ) {
    cerr << "Error[MffcPropCone::make_prop_condition()]: "
	 << root->id() << " is not within the MFFC" << endl;
    return {};
  }

  SizeType ffr_num = mElemArray.size();
  if ( ffr_num > 1 ) {
    // FFR の根の出力に故障を挿入する．
    int ffr_id = mElemPosMap.at(root->id());
    vector<SatLiteral> assumptions;
    assumptions.reserve(ffr_num);
    for ( SizeType i = 0; i < ffr_num; ++ i ) {
      auto evar = mElemVarArray[i];
      bool inv = (i != ffr_id);
      auto lit = evar * inv;
      assumptions.push_back(lit);
    }
    return assumptions;
  }
  else {
    return {};
  }
}

END_NAMESPACE_DRUID_STRUCTENC
