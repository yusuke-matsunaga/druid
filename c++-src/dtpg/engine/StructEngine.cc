
/// @file StructEngine.cc
/// @brief StructEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/StructEngine.h"
#include "types/TpgNetwork.h"
#include "GateEnc.h"
#include "Justifier.h"


//#define DEBUG_DTPG
#define DEBUG_OUT std::cout


BEGIN_NONAMESPACE
//#define DEBUG_DTPG
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
  const JsonValue& option
) : mNetwork{network},
    mSolver(SatInitParam(get_option(option, "sat_param"))),
    mGvarMap(network.node_num()),
    mHvarMap(network.node_num()),
    mJustifier{Justifier::new_obj(network, get_option(option, "justifier"))}
{
}

// @brief デストラクタ
StructEngine::~StructEngine()
{
}

// @brief SubEnc を追加する．
void
StructEngine::add_subenc(
  std::unique_ptr<SubEnc>&& enc
)
{
  mSubEncCandList.push_back(enc.get());
  enc->mEngine = this;
  enc->init();
  mCurNodeCandList.reserve(mCurNodeCandList.size() + enc->node_list().size());
  for ( auto node: enc->node_list() ) {
    mCurNodeCandList.push_back(node);
  }
  mPrevNodeCandList.reserve(mPrevNodeCandList.size() + enc->prev_node_list().size());
  for ( auto node: enc->prev_node_list() ) {
    mPrevNodeCandList.push_back(node);
  }
  mSubEncList.push_back(std::move(enc));
  mState = DIRTY;
}

// @brief 現時刻で考慮するノードを追加する．
void
StructEngine::add_cur_node(
  const TpgNode& node
)
{
  mCurNodeCandList.push_back(node);
  mState = DIRTY;
}

// @brief 1時刻前で考慮するノードを追加する．
void
StructEngine::add_prev_node(
  const TpgNode& node
)
{
  mPrevNodeCandList.push_back(node);
  mState = DIRTY;
}

// @brief 回路の構造を表すCNFを生成する．
void
StructEngine::_update()
{
  mTimer.reset();
  mTimer.start();

  mState = UPDATING;

  bool has_prev_state = mNetwork.has_prev_state();

  // 関係するノードのリストを作る．
  TpgNodeList new_dff_input_list;
  auto new_node_list = mNetwork.get_tfi_list(
    mCurNodeCandList,
    [&](const TpgNode& node) {
      if ( has_prev_state && node.is_dff_output() ) {
	auto alt_node = node.alt_node();
	new_dff_input_list.push_back(alt_node);
	if ( new_dff_input_list.network() != alt_node.network() ) {
	  abort();
	}
      }
    });

  TpgNodeList new_prev_node_list;
  if ( has_prev_state ) {
    auto prev_list = new_dff_input_list;
    for ( auto node: mPrevNodeCandList ) {
      prev_list.push_back(node);
    }
    new_prev_node_list = mNetwork.get_tfi_list(prev_list);
  }

  // 変数を割り当てる．
  TpgNodeList new_node_list2;
  new_node_list2.reserve(new_node_list.size());
  std::unordered_set<SizeType> new_node_mark;
  for ( auto node: new_node_list ) {
    if ( mGvarMap(node) == SatLiteral::X ) {
      auto glit = new_variable(true);
      mGvarMap.set_vid(node, glit);
      new_node_list2.push_back(node);
      new_node_mark.emplace(node.id());

      if ( debug_base_enc ) {
	DEBUG_OUT << node
		  << ": gvar = " << glit
		  << std::endl;
      }
    }
  }
  TpgNodeList new_prev_node_list2;
  new_prev_node_list2.reserve(new_prev_node_list.size());
  for ( auto node: new_prev_node_list ) {
    if ( mHvarMap(node) == SatLiteral::X ) {
      auto hlit = new_variable(true);
      mHvarMap.set_vid(node, hlit);
      new_prev_node_list2.push_back(node);

      if ( debug_base_enc ) {
	DEBUG_OUT << node
		  << ": hvar = " << hlit
		  << std::endl;
      }
    }
  }

  // 現時刻の値の関係を表すCNFを作る．
  GateEnc gvar_enc(mSolver, mGvarMap);
  for ( auto node: new_node_list2 ) {
    gvar_enc.make_cnf(node);
  }

  // 1時刻前の値の関係を表すCNFを作る．
  GateEnc hvar_enc(mSolver, mHvarMap);
  for ( auto node: new_prev_node_list2 ) {
    hvar_enc.make_cnf(node);
  }

  // DFF の入力と出力の関係を表すCNFを作る．
  for ( auto node: new_dff_input_list ) {
    auto onode = node.alt_node();
    if ( new_node_mark.count(onode.id()) > 0 ) {
      auto olit = gvar(onode);
      auto ilit = hvar(node);
      mSolver.add_buffgate(olit, ilit);
    }
  }

  for ( auto sub: mSubEncCandList ) {
    sub->make_cnf();
  }

  mSubEncCandList.clear();
  mCurNodeCandList.clear();
  mPrevNodeCandList.clear();
  mState = STABLE;

  mTimer.stop();
  mCnfTime += mTimer.get_time();
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
  const std::vector<SatLiteral>& assumptions
)
{
  update();
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
  update();
  auto node = assign.node();
  bool inv = !assign.val(); // 0 の時が inv = true
  auto vid = (assign.time() == 0) ? hvar(node) : gvar(node);
  if ( vid == SatLiteral::X ) {
    abort();
    throw std::invalid_argument{"assign is not registered"};
  }
  return vid * inv;
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
std::vector<SatLiteral>
StructEngine::conv_to_literal_list(
  const AssignList& assign_list
)
{
  std::vector<SatLiteral> ans_list;
  ans_list.reserve(assign_list.size());
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    ans_list.push_back(lit);
  }
  return ans_list;
}

BEGIN_NONAMESPACE

void
dfs(
  const Expr& expr,
  std::unordered_set<SizeType>& input_id_set
)
{
  if ( expr.is_constant() ) {
    return;
  }
  if ( expr.is_literal() ) {
    auto varid = expr.varid();
    if ( input_id_set.count(varid) == 0 ) {
      input_id_set.emplace(varid);
    }
    return;
  }
  for ( auto& expr1: expr.operand_list() ) {
    dfs(expr1, input_id_set);
  }
}

END_NONAMESPACE

// @brief 与えられた論理式を充足させるCNF式を作る．
std::vector<SatLiteral>
StructEngine::expr_to_cnf(
  const Expr& expr
)
{
  // Expr の変数番号とリテラルの対応表を作る．
  std::unordered_set<SizeType> input_id_set;
  dfs(expr, input_id_set);
  std::unordered_map<SizeType, SatLiteral> lit_map;
  for ( auto varid: input_id_set ) {
    auto node_id = varid / 2;
    auto time = varid % 2;
    auto node = mNetwork.node(node_id);
    auto lit = (time == 0) ? hvar(node) : gvar(node);
    lit_map.emplace(varid, lit);
  }
  return solver().add_expr(expr, lit_map);
}

// @brief 値を返す．
bool
StructEngine::val(
  const TpgNode& node,
  int time
)
{
  auto lit = (time == 0) ? hvar(node) : gvar(node);
  return mSolver.model()[lit] == SatBool3::True;
}


//////////////////////////////////////////////////////////////////////
// クラス SubEnc
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE
static TpgNodeList dummy;
END_NONAMESPACE

// @brief 関連するノードのリストを返す．
const TpgNodeList&
SubEnc::node_list() const
{
  return dummy;
}

// @brief 1時刻前の値に関連するノードのリストを返す．
const TpgNodeList&
SubEnc::prev_node_list() const
{
  return dummy;
}

END_NAMESPACE_DRUID
