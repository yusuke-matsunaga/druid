
/// @file StructEngine.cc
/// @brief StructEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "StructEngine.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"
#include "Justifier.h"


//#define DEBUG_DTPG
#define DEBUG_OUT cout


BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_base_enc = 1;
#else
int debug_base_enc = 0;
#endif
END_NONAMESPACE

BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス StructEngine
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

JsonValue
get_option(
  const JsonValue& option,
  const char* keyword
)
{
  if ( option.is_object() && option.has_key(keyword) ) {
    return option.get(keyword);
  }
  return JsonValue{};
}

END_NONAMESPACE

// @brief コンストラクタ
StructEngine::StructEngine(
  const TpgNetwork& network,
  const std::vector<SubEnc*>& subenc_list,
  const std::vector<const TpgNode*>& ex_node_list,
  const std::vector<const TpgNode*>& ex_prev_node_list,
  const JsonValue& option
) : mNetwork{network},
    mSolver(SatInitParam{get_option(option, "sat_param")}),
    mGvarMap(network.node_num()),
    mHvarMap(network.node_num()),
    mJustifier{Justifier::new_obj(network, get_option(option, "justifier"))}
{
  mSubEncList.reserve(subenc_list.size());
  for ( auto enc: subenc_list ) {
    mSubEncList.push_back(std::unique_ptr<SubEnc>{enc});
    enc->mEngine = this;
    enc->init();
  }
  _make_cnf(ex_node_list, ex_prev_node_list);
}

// @brief デストラクタ
StructEngine::~StructEngine()
{
}

// @brief 回路の構造を表すCNFを生成する．
void
StructEngine::_make_cnf(
  const std::vector<const TpgNode*>& ex_node_list,
  const std::vector<const TpgNode*>& ex_prev_node_list
)
{
  mTimer.reset();
  mTimer.start();

  // 関係するノードのリストを作る．
  auto node_list = ex_node_list;
  for ( auto& sub: mSubEncList ) {
    auto& node_list1 = sub->node_list();
    node_list.insert(node_list.end(), node_list1.begin(), node_list1.end());
  }

  bool has_prev_state = mNetwork.has_prev_state();
  mCurNodeList = TpgNodeSet::get_tfi_list(
    mNetwork.node_num(),
    node_list,
    [&](const TpgNode* node) {
      if ( has_prev_state && node->is_dff_output() ) {
	auto alt_node = node->alt_node();
	mDffInputList.push_back(alt_node);
      }
    });

  if ( has_prev_state ) {
    auto prev_list = mDffInputList;
    prev_list.insert(prev_list.end(),
		     ex_prev_node_list.begin(), ex_prev_node_list.end());
    for ( auto& sub: mSubEncList ) {
      auto& node_list1 = sub->prev_node_list();
      prev_list.insert(prev_list.end(), node_list1.begin(), node_list1.end());
    }
    mPrevNodeList = TpgNodeSet::get_tfi_list(
      mNetwork.node_num(),
      prev_list);
  }

  // 変数を割り当てる．
  for ( auto node: mCurNodeList ) {
    auto glit = new_variable(true);
    mGvarMap.set_vid(node, glit);

    if ( debug_base_enc ) {
      DEBUG_OUT << "Node#" << node->id()
		<< ": gvar = " << glit << endl;
    }
  }
  for ( auto node: mPrevNodeList ) {
    auto hlit = new_variable(true);
    mHvarMap.set_vid(node, hlit);

    if ( debug_base_enc ) {
      DEBUG_OUT << "Node#" << node->id()
		<< ": hvar = " << hlit << endl;
    }
  }

  // 現時刻の値の関係を表すCNFを作る．
  GateEnc gvar_enc(mSolver, mGvarMap);
  for ( auto node: mCurNodeList ) {
    gvar_enc.make_cnf(node);
  }

  // 1時刻前の値の関係を表すCNFを作る．
  GateEnc hvar_enc(mSolver, mHvarMap);
  for ( auto node: mPrevNodeList ) {
    hvar_enc.make_cnf(node);
  }

  // DFF の入力と出力の関係を表すCNFを作る．
  for ( auto node: mDffInputList ) {
    auto onode = node->alt_node();
    auto olit = gvar(onode);
    auto ilit = hvar(node);
    mSolver.add_buffgate(olit, ilit);
  }

  for ( auto& sub: mSubEncList ) {
    sub->make_cnf();
  }

  mTimer.stop();
  mCnfTime = mTimer.get_time();
}

// @brief 変数を作る．
SatLiteral
StructEngine::new_variable(
  bool decision
)
{
  return mSolver.new_variable(decision);
}

// @brief SAT問題を解く
SatBool3
StructEngine::solve(
  const vector<SatLiteral>& assumptions
)
{
  return mSolver.solve(assumptions);
}

// @brief 現在の内部状態を得る．
SatStats
StructEngine::get_stats() const
{
  return mSolver.get_stats();
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
AssignList
StructEngine::justify(
  const AssignList& assign_list
)
{
  auto& model = mSolver.model();
  if ( mNetwork.has_prev_state() ) {
    return mJustifier->justify(assign_list, mHvarMap, mGvarMap, model);
  }
  else {
    return mJustifier->justify(assign_list, mGvarMap, model);
  }
}

// @brief 現在の外部入力の割当を得る．
AssignList
StructEngine::get_pi_assign()
{
  AssignList pi_assign;
  if ( mNetwork.has_prev_state() ) {
    for ( auto node: mNetwork.ppi_list() ) {
      auto v = val(node, 0);
      pi_assign.add(node, 0, v);
    }
    for ( auto node: mNetwork.input_list() ) {
      auto v = val(node, 1);
      pi_assign.add(node, 1, v);
    }
  }
  else {
    for ( auto node: mNetwork.ppi_list() ) {
      auto v = val(node, 1);
      pi_assign.add(node, 1, v);
    }
  }
  return pi_assign;
}

// @brief 値割り当てを対応するリテラルに変換する．
SatLiteral
StructEngine::conv_to_literal(
  Assign assign
)
{
  auto node = assign.node();
  bool inv = !assign.val(); // 0 の時が inv = true
  auto vid = (assign.time() == 0) ? hvar(node) : gvar(node);
  ASSERT_COND( vid != SatLiteral::X );
  return vid * inv;
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
vector<SatLiteral>
StructEngine::conv_to_literal_list(
  const AssignList& assign_list
)
{
  vector<SatLiteral> ans_list;
  ans_list.reserve(assign_list.size());
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    ans_list.push_back(lit);
  }
  return ans_list;
}

// @brief 与えられた論理式を充足させるCNF式を作る．
vector<SatLiteral>
StructEngine::expr_to_cnf(
  const Expr& expr
)
{
  if ( expr.is_zero() ) {
    // 充足不能
    throw std::invalid_argument{"expr is zero"};
  }
  if ( expr.is_one() ) {
    // 無条件で充足している．
    return {};
  }
  if ( expr.is_literal() ) {
    auto vid = expr.varid();
    auto node_id = vid / 2;
    auto time = vid % 2;
    auto node = mNetwork.node(node_id);
    auto lit = (time == 0) ? hvar(node) : gvar(node);
    if ( expr.is_nega_literal() ) {
      lit = ~lit;
    }
    return {lit};
  }
  if ( expr.is_and() ) {
    vector<SatLiteral> lits;
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = expr_to_cnf(expr1);
      lits.insert(lits.end(), lits1.begin(), lits1.end());
    }
    return lits;
  }
  if ( expr.is_or() ) {
    auto new_lit = solver().new_variable(false);
    vector<SatLiteral> lits;
    lits.reserve(expr.operand_num() + 1);
    lits.push_back(~new_lit);
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = expr_to_cnf(expr1);
      if ( lits1.empty() ) {
	continue;
      }
      if ( lits1.size() == 1 ) {
	auto lit1 = lits1.front();
	lits.push_back(lit1);
      }
      else {
	auto lit1 = solver().new_variable(false);
	for ( auto lit: lits1 ) {
	  solver().add_clause(~lit1, lit);
	}
	lits.push_back(lit1);
      }
    }
    solver().add_clause(lits);
    return {new_lit};
  }
  if ( expr.is_xor() ) {
    // EXOR は両極性を必要とするので非常に効率が悪い．
    auto new_lit = solver().new_variable(false);
    vector<SatLiteral> lits;
    lits.reserve(expr.operand_num());
    for ( auto& expr1: expr.operand_list() ) {
      auto lit1 = solver().new_variable(false);
      auto lits1 = expr_to_cnf(expr1);
      solver().add_andgate(lit1, lits1);
      lits.push_back(lit1);
    }
    solver().add_xorgate(new_lit, lits);
    return {new_lit};
  }
  throw std::invalid_argument{"unexpected error"};
  return {};
}

// @brief 値を返す．
bool
StructEngine::val(
  const TpgNode* node,
  int time
) const
{
  auto lit = (time == 0) ? hvar(node) : gvar(node);
  return mSolver.model()[lit] == SatBool3::True;
}


//////////////////////////////////////////////////////////////////////
// クラス SubEnc
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE
static vector<const TpgNode*> dummy;
END_NONAMESPACE

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
SubEnc::node_list() const
{
  return dummy;
}

// @brief 1時刻前の値に関連するノードのリストを返す．
const vector<const TpgNode*>&
SubEnc::prev_node_list() const
{
  return dummy;
}

END_NAMESPACE_DRUID
