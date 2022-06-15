
/// @file DtpgMFFC.cc
/// @brief DtpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgMFFC.h"

#include "TpgFault.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateType.h"
#include "GateEnc.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/Range.h"
#include "ym/SatTseitinEnc.h"

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
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] mffc 故障伝搬の起点となる MFFC
// @param[in] solver_type SATソルバの実装タイプ
DtpgMFFC::DtpgMFFC(const TpgNetwork& network,
		   FaultType fault_type,
		   const TpgMFFC& mffc,
		   const string& just_type,
		   const SatSolverType& solver_type) :
  DtpgEngine(network, fault_type, mffc.root(), just_type, solver_type),
  mElemArray(mffc.ffr_num()),
  mElemVarArray(mffc.ffr_num())
{
  int ffr_id = 0;
  for ( auto ffr: mffc.ffr_list() ) {
    mElemArray[ffr_id] = ffr->root();
    ASSERT_COND( ffr->root() != nullptr );
    mElemPosMap.emplace(ffr->root()->id(), ffr_id);
    ++ ffr_id;
  }

  cnf_begin();

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  {
    vector<SatLiteral> odiff;
    odiff.reserve(output_list().size());
    for ( auto node: output_list() ) {
      SatLiteral dlit(dvar(node));
      odiff.push_back(dlit);
    }
    solver().add_clause(odiff);

    if ( !root_node()->is_ppo() ) {
      // root_node() の dlit が1でなければならない．
      SatLiteral dlit0(dvar(root_node()));
      solver().add_clause(dlit0);
    }
  }

  gen_mffc_cnf();

  cnf_end();
}

// @brief デストラクタ
DtpgMFFC::~DtpgMFFC()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
DtpgResult
DtpgMFFC::gen_pattern(const TpgFault* fault)
{
  vector<SatLiteral> assumptions;

  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  if ( ffr_root != root_node() ) {
    // root のある FFR を活性化する条件を作る．
    if ( mElemPosMap.count(ffr_root->id()) == 0 ) {
      cerr << "Error[DtpgMFFC::dtpg()]: "
	   << ffr_root->id() << " is not within the MFFC" << endl;
      return DtpgResult();
    }

    int ffr_id = mElemPosMap.at(ffr_root->id());
    int ffr_num = mElemArray.size();
    if ( ffr_num > 1 ) {
      // FFR の根の出力に故障を挿入する．
      assumptions.reserve(ffr_num);
      for ( auto i: Range(ffr_num) ) {
	auto evar = mElemVarArray[i];
	bool inv = (i != ffr_id);
	assumptions.push_back(SatLiteral(evar, inv));
      }
    }
  }

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  conv_to_assumptions(ffr_cond, assumptions);

  SatBool3 sat_res = solve(assumptions);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition(ffr_root);
    suf_cond.merge(ffr_cond);
    TestVector testvect = backtrace(suf_cond);
    return DtpgResult(testvect);
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

// @brief 十分条件を取り出す．
// @param[in] root 対象の故障のあるFFRの根のノード
// @return 十分条件を表す割当リストを返す．
NodeValList
DtpgMFFC::get_sufficient_condition(const TpgNode* root)
{
  extern
  NodeValList
  extract(const TpgNode* root,
	  const VidMap& gvar_map,
	  const VidMap& fvar_map,
	  const SatModel& model);

  return extract(root, gvar_map(), fvar_map(), sat_model());
}

// @brief 複数の十分条件を取り出す．
// @param[in] root 対象の故障のあるFFRの根のノード
//
// FFR内の故障伝搬条件は含まない．
Expr
DtpgMFFC::get_sufficient_conditions(const TpgNode* root)
{
  extern
  Expr
  extract_all(const TpgNode* root,
	      const VidMap& gvar_map,
	      const VidMap& fvar_map,
	      const SatModel& model);

  return extract_all(root, gvar_map(), fvar_map(), sat_model());
}

// @brief mffc 内の影響が root まで伝搬する条件のCNF式を作る．
void
DtpgMFFC::gen_mffc_cnf()
{
  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    auto cvar = solver().new_variable();
    mElemVarArray[i] = cvar;

    solver().freeze_literal(SatLiteral(cvar));

    if ( debug_mffc ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << cvar << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  unordered_map<int, int> ffr_map;
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    ffr_map.emplace(node->id(), i);
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	auto var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffc ) {
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
	auto var = solver().new_variable();
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
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    auto fvar = solver().new_variable();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  GateEnc fval_enc(solver(), fvar_map());
  for ( int rpos = 0; rpos < node_list.size(); ++ rpos ) {
    const TpgNode* node = node_list[rpos];
    auto ovar = fvar(node);
    if ( ffr_map.count(node->id()) > 0 ) {
      int ffr_pos = ffr_map.at(node->id());
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[ffr_pos]
      ovar = solver().new_variable();
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

// @brief 故障挿入回路のCNFを作る．
// @param[in] ffr_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
DtpgMFFC::inject_fault(int ffr_pos,
		       SatLiteral ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[ffr_pos]);
  const TpgNode* node = mElemArray[ffr_pos];
  SatLiteral olit(fvar(node));

  SatTseitinEnc enc{solver()};
  enc.add_xorgate(lit1, lit2, olit);

  if ( debug_mffc ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[ffr_pos] << endl;
  }
}

END_NAMESPACE_DRUID
