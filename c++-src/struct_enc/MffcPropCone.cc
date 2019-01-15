
/// @file MffcPropCone.cc
/// @brief MffcPropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017 Yusuke Matsunaga
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
// @param[in] struct_sat StructEnc ソルバ
// @param[in] mffc MFFC の情報
// @param[in] block_node ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
//
// ブロックノードより先のノードは含めない．
// 通常 block_node は nullptr か root_node の dominator
// となっているはず．
MffcPropCone::MffcPropCone(StructEnc& struct_sat,
			   const TpgMFFC& mffc,
			   const TpgNode* block_node,
			   bool detect) :
  PropCone(struct_sat, mffc.root(), block_node, detect),
  mElemArray(mffc.ffr_num()),
  mElemVarArray(mffc.ffr_num())
{
  for (int i = 0; i < mffc.ffr_num(); ++ i ) {
    const TpgFFR* ffr = mffc.ffr(i);
    mElemArray[i] = ffr->root();
    ASSERT_COND( ffr->root() != nullptr );
    mElemPosMap.add(ffr->root()->id(), i);
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
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    mElemVarArray[i] = solver().new_variable();

    if ( debug_mffccone ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  HashMap<int, int> ffr_map;
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    ffr_map.add(node->id(), i);
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  for ( int rpos = 0; rpos < node_list.size(); ++ rpos ) {
    const TpgNode* node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
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
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  GateEnc gate_enc(solver(), fvar_map());
  for ( int rpos = 0; rpos < node_list.size(); ++ rpos ) {
    const TpgNode* node = node_list[rpos];
    SatVarId ovar = fvar(node);
    int ffr_pos;
    if ( ffr_map.find(node->id(), ffr_pos) ) {
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
// @param[in] ffr_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
MffcPropCone::inject_fault(int ffr_pos,
			   SatVarId ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[ffr_pos]);
  const TpgNode* node = mElemArray[ffr_pos];
  SatLiteral olit(fvar(node));

  solver().add_xorgate_rel(lit1, lit2, olit);

  if ( debug_mffccone ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[ffr_pos] << endl;
  }
}

// @brief 故障の影響伝搬させる条件を作る．
// @param[in] root 起点となるノード
// @param[out] assumptions 結果の仮定を表すリテラルのリスト
void
MffcPropCone::make_prop_condition(const TpgNode* root,
				  vector<SatLiteral>& assumptions)
{
  // root のある FFR を活性化する条件を作る．
  int ffr_id;
  bool stat = mElemPosMap.find(root->id(), ffr_id);
  if ( !stat ) {
    cerr << "Error[MffcPropCone::make_prop_condition()]: "
	 << root->id() << " is not within the MFFC" << endl;
    return;
  }

  int ffr_num = mElemArray.size();
  if ( ffr_num > 1 ) {
    // FFR の根の出力に故障を挿入する．
    assumptions.reserve(assumptions.size() + ffr_num);
    for (int i = 0; i < ffr_num; ++ i) {
      SatVarId evar = mElemVarArray[i];
      bool inv = (i != ffr_id);
      assumptions.push_back(SatLiteral(evar, inv));
    }
  }
}

END_NAMESPACE_DRUID_STRUCTENC
