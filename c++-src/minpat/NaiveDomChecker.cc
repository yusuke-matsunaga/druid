
/// @file NaiveDomChecker.cc
/// @brief NaiveDomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveDomChecker.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
NaiveDomChecker::NaiveDomChecker(
  const TpgNetwork& network,
  const TpgFault* fault1,
  const TpgFault* fault2,
  const SatInitParam& init_param
) : mSolver{init_param},
    mNetwork{network},
    mFault1{fault1},
    mFault2{fault2},
    mRoot1{fault1->origin_node()},
    mRoot2{fault2->origin_node()},
    mMarkArray(mNetwork.node_num(), 0U),
    mHvarMap(mNetwork.node_num()),
    mGvarMap(mNetwork.node_num()),
    mFvarMap1(mNetwork.node_num()),
    mFvarMap2(mNetwork.node_num()),
    mDvarMap(mNetwork.node_num())
{
  vector<const TpgNode*> tmp_list;
  mTfoList1 = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot1,
				       [&](const TpgNode* node) {
					 if ( node->is_ppo() ) {
					   mOutputList1.push_back(node);
					 }
					 tmp_list.push_back(node);
				       });

  mTfoList2 = TpgNodeSet::get_tfo_list(mNetwork.node_num(), mRoot2,
				       [&](const TpgNode* node) {
					 if ( node->is_ppo() ) {
					   mOutputList2.push_back(node);
					 }
					 tmp_list.push_back(node);
				       });

  bool has_prev_state = mNetwork.has_prev_state();
  mTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list,
				      [&](const TpgNode* node) {
					if ( has_prev_state &&
					     node->is_dff_output() ) {
					  auto alt_node = node->alt_node();
					  mDffInputList.push_back(alt_node);
					}
				      });
  if ( has_prev_state ) {
    auto tmp_list = mDffInputList;
    if ( mRoot1->is_dff_output() ) {
      tmp_list.push_back(mRoot1->alt_node());
    }
    if ( mRoot2->is_dff_output() ) {
      tmp_list.push_back(mRoot2->alt_node());
    }
    tmp_list.push_back(mRoot1);
    tmp_list.push_back(mRoot2);
    mPrevTfiList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list);
  }

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路1の CNF を生成
  gen_faulty_cnf1();

  // 故障回路2の CNF を生成
  gen_faulty_cnf2();

}

// @brief デストラクタ
NaiveDomChecker::~NaiveDomChecker()
{
}

// @brief チェックする．
bool
NaiveDomChecker::check()
{
  return solver().solve() == SatBool3::False;
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
NaiveDomChecker::prepare_vars()
{
  // 正常回路の変数を作る．
  for ( auto node: mTfiList ) {
    auto gvar = new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap1.set_vid(node, gvar);
    mFvarMap2.set_vid(node, gvar);
  }

  // 故障回路1の変数を作る．
  for ( auto node: mTfoList1 ) {
    auto fvar = new_variable();
    mFvarMap1.set_vid(node, fvar);

    auto dvar = new_variable(false);
    mDvarMap.set_vid(node, dvar);
  }

  // 故障回路2の変数を作る．
  for ( auto node: mTfoList2 ) {
    auto fvar = new_variable();
    mFvarMap2.set_vid(node, fvar);
  }

  // 1時刻前の正常回路の変数を作る．
  for ( auto node: mPrevTfiList ) {
    auto hvar = new_variable();
    mHvarMap.set_vid(node, hvar);
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
NaiveDomChecker::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc{mSolver, mGvarMap};
  for ( auto node: mTfiList ) {
    {
      auto olit = gvar(node);
      if ( olit == SatLiteral::X ) {
	cout << node->str() << ": gvar = X" << endl;
	abort();
      }
      for ( auto inode: node->fanin_list() ) {
	auto ilit = gvar(inode);
	if ( ilit == SatLiteral::X ) {
	  cout << inode->str() << ": gvar = X" << endl;
	  abort();
	}
      }
    }
    gval_enc.make_cnf(node);
  }

  GateEnc hval_enc{mSolver, mHvarMap};
  for ( auto node: mPrevTfiList ) {
    if ( hvar(node) == SatLiteral::X ) {
      cout << node->str() << ": hvar = X" << endl;
      abort();
    }
    hval_enc.make_cnf(node);
  }

  // DFF の入力と出力の値を一致させる．
  for ( auto node: mDffInputList ) {
    auto onode = node->alt_node();
    auto ilit = hvar(node);
    auto olit = gvar(onode);
    mSolver.add_buffgate(olit, ilit);
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
NaiveDomChecker::gen_faulty_cnf1()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap1};
  for ( auto node: mTfoList1 ) {
    if ( node != mRoot1 ) {
      fval_enc.make_cnf(node);
    }
    make_dchain_cnf(node);
  }
  {
    auto glit = gvar(mRoot1);
    auto flit = fvar2(mRoot1);
    // flit が glit と異なるのは fault->excitation_condition()
    // が成り立っている時．
    auto dlit = new_variable(false);
    mSolver.add_xorgate(dlit, glit, flit);
    auto ex_cond = mFault1->excitation_condition();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ex_cond.size() + 1);
    for ( auto nv: ex_cond ) {
      auto lit = conv_to_literal(nv);
      mSolver.add_clause(lit, ~dlit);
      tmp_lits.push_back(~lit);
    }
    tmp_lits.push_back(dlit);
    mSolver.add_clause(tmp_lits);
  }

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  vector<SatLiteral> odiff;
  odiff.reserve(mOutputList1.size());
  for ( auto node: mOutputList1 ) {
    auto dlit = dvar(node);
    odiff.push_back(dlit);
  }
  solver().add_clause(odiff);

  if ( !mRoot1->is_ppo() ) {
    // mRoot1 の dlit が1でなければならない．
    auto dlit0 = dvar(mRoot1);
    solver().add_clause(dlit0);
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
NaiveDomChecker::gen_faulty_cnf2()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc{mSolver, mFvarMap2};
  for ( auto node: mTfoList2 ) {
    if ( node != mRoot1 ) {
      fval_enc.make_cnf(node);
    }
  }

  for ( auto node: mOutputList2 ) {
    auto glit = gvar(node);
    auto flit = fvar2(node);
    mSolver.add_clause( glit, ~flit);
    mSolver.add_clause(~glit,  flit);
  }
  {
    auto glit = gvar(mRoot2);
    auto flit = fvar2(mRoot2);
    // flit が glit と異なるのは fault->excitation_condition()
    // が成り立っている時．
    auto dlit = new_variable(false);
    mSolver.add_xorgate(dlit, glit, flit);
    auto ex_cond = mFault2->excitation_condition();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ex_cond.size() + 1);
    for ( auto nv: ex_cond ) {
      auto lit = conv_to_literal(nv);
      mSolver.add_clause(lit, ~dlit);
      tmp_lits.push_back(~lit);
    }
    tmp_lits.push_back(dlit);
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
NaiveDomChecker::make_dchain_cnf(
  const TpgNode* node
)
{
  auto glit = mGvarMap(node);
  auto flit = mFvarMap1(node);
  auto dlit = mDvarMap(node);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto odlit = mDvarMap(node->fanout(0));
      mSolver.add_clause(~dlit, odlit);
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	tmp_lits.push_back(SatLiteral(mDvarMap(onode)));
      }
      tmp_lits.push_back(~dlit);
      mSolver.add_clause(tmp_lits);

      auto imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	auto odlit = mDvarMap(imm_dom);
	mSolver.add_clause(~dlit, odlit);
      }
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
vector<SatLiteral>
NaiveDomChecker::conv_to_literals(
  const NodeTimeValList& nv_list
)
{
  vector<SatLiteral> tmp_list;
  tmp_list.reserve(nv_list.size());
  for ( auto nv: nv_list ) {
    auto lit = conv_to_literal(nv);
    tmp_list.push_back(lit);
  }
  return tmp_list;
}

END_NAMESPACE_DRUID
