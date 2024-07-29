
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
    mJustifier{Justifier::new_obj(network, get_option(option, "justifier"))}
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
BaseEnc::make_cnf(
  const vector<const TpgNode*>& cur_node_list,
  const vector<const TpgNode*>& prev_node_list
)
{
  mTimer.reset();
  mTimer.start();

  // 関係するノードのリストを作る．
  vector<const TpgNode*> node_list{cur_node_list};
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

  if ( has_prev_state ) {
    vector<const TpgNode*> prev_list{mDffInputList};
    prev_list.insert(prev_list.end(), prev_node_list.begin(), prev_node_list.end());
    for ( auto sub: mSubEncList ) {
      auto& node_list1 = sub->prev_node_list();
      prev_list.insert(prev_list.end(), node_list1.begin(), node_list1.end());
    }
    mPrevNodeList = TpgNodeSet::get_tfi_list(
      mNetwork.node_num(),
      prev_list);
  }

  // 変数を割り当てる．
  for ( auto node: mCurNodeList ) {
    auto glit = solver().new_variable(true);
    mGvarMap.set_vid(node, glit);

    if ( debug_base_enc ) {
      DEBUG_OUT << "Node#" << node->id()
		<< ": gvar = " << glit << endl;
    }
  }
  for ( auto node: mPrevNodeList ) {
    auto hlit = solver().new_variable(true);
    mHvarMap.set_vid(node, hlit);

    if ( debug_base_enc ) {
      DEBUG_OUT << "Node#" << node->id()
		<< ": hvar = " << hlit << endl;
    }
  }

  // 現時刻の値の関係を表すCNFを作る．
  GateEnc gvar_enc{mSolver, mGvarMap};
  for ( auto node: mCurNodeList ) {
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
    gvar_enc.make_cnf(node);
  }

  // 1時刻前の値の関係を表すCNFを作る．
  GateEnc hvar_enc{mSolver, mHvarMap};
  for ( auto node: mPrevNodeList ) {
    {
      auto olit = hvar(node);
      if ( olit == SatLiteral::X ) {
	cout << node->str() << ": hvar = X" << endl;
	abort();
      }
      for ( auto inode: node->fanin_list() ) {
	auto ilit = hvar(inode);
	if ( ilit == SatLiteral::X ) {
	  cout << inode->str() << ": hvar = X" << endl;
	  abort();
	}
      }
    }
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

  mTimer.stop();
  mCnfTime = mTimer.get_time();
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
AssignList
BaseEnc::justify(
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
BaseEnc::get_pi_assign()
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
BaseEnc::conv_to_literal(
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
BaseEnc::conv_to_literal_list(
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

// @brief 値を返す．
bool
BaseEnc::val(
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
