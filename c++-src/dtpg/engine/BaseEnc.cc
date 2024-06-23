
/// @file BaseEnc.cc
/// @brief BaseEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BaseEnc.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"


BEGIN_NAMESPACE_DRUID

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
BaseEnc::BaseEnc(
  const TpgNetwork& network,
  const JsonValue& option
) : mNetwork{network},
    mSolver{SatInitParam{get_option(option, "sat_param")}},
    mGvarMap{network.node_num()},
    mHvarMap{network.node_num()},
    mJustifier{network, get_option(option, "justifier")}
{
}

// @brief デストラクタ
BaseEnc::~BaseEnc()
{
  for ( auto sub: mSubEncList ) {
    delete sub;
  }
}

// @brief 回路の構造を表すCNFを生成する．
void
BaseEnc::make_cnf()
{
  vector<const TpgNode*> node_list;
  for ( auto sub: mSubEncList ) {
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

  mPrevNodeList = TpgNodeSet::get_tfi_list(
    mNetwork.node_num(),
    mDffInputList);

  // 現時刻の値の関係を表すCNFを作る．
  GateEnc gvar_enc{mSolver, mGvarMap};
  for ( auto node: mCurNodeList ) {
    gvar_enc.make_cnf(node);
  }

  // 1時刻前の値の関係を表すCNFを作る．
  GateEnc hvar_enc{mSolver, mHvarMap};
  for ( auto node: mPrevNodeList ) {
    hvar_enc.make_cnf(node);
  }

  // DFF の入力と出力の関係を表すCNFを作る．
  for ( auto node: mDffInputList ) {
    auto onode = node->alt_node();
    auto olit = gvar(onode);
    auto ilit = hvar(node);
    if ( olit == SatLiteral::X ) {
      cout << onode->str() << ": gvar = X" << endl;
      abort();
    }
    if ( ilit == SatLiteral::X ) {
      cout << node->str() << ": hvar = X" << endl;
      abort();
    }
    mSolver.add_buffgate(olit, ilit);
  }

  for ( auto sub: mSubEncList ) {
    sub->make_cnf();
  }
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
NodeTimeValList
BaseEnc::justify(
  const NodeTimeValList& assign_list
)
{
  auto& model = mSolver.model();
  return mJustifier(assign_list, mHvarMap, mGvarMap, model);
}

// @brief 値割り当てを対応するリテラルに変換する．
SatLiteral
BaseEnc::conv_to_literal(
  NodeTimeVal assign
)
{
  auto node = assign.node();
  bool inv = !assign.val(); // 0 の時が inv = true
  auto vid = (assign.time() == 0) ? hvar(node) : gvar(node);
  return vid * inv;
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
vector<SatLiteral>
BaseEnc::conv_to_literal_list(
  NodeTimeValList& assign_list
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

END_NAMESPACE_DRUID
