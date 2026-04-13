
/// @file BoolDiffEnc.cc
/// @brief BoolDiffEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BoolDiffEnc.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNetwork.h"
#include "GateEnc.h"
#include "Extractor.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
BoolDiffEnc::BoolDiffEnc(
  const TpgNode& root,
  const ConfigParam& option
) : BoolDiffEnc(root, TpgNodeList(), option)
{
}

// @brief コンストラクタ
BoolDiffEnc::BoolDiffEnc(
  const TpgNode& root,
  const TpgNode& output,
  const ConfigParam& option
) : BoolDiffEnc(root, TpgNodeList(std::vector<TpgNode>{output}), option)
{
}

// @brief コンストラクタ
BoolDiffEnc::BoolDiffEnc(
  const TpgNode& root,
  const TpgNodeList& output_list,
  const ConfigParam& option
) : mRoot{root},
    mOutputMark(root.network().node_num(), false),
    mExtractor{Extractor::new_impl(JsonValue(option.get_value("extractor")))}
{
  auto network = root.network();
  for ( auto node: output_list ) {
    mOutputMark[node.id()] = true;
  }
  mTfoList = network.get_tfo_list(
    mRoot,
    [&](const TpgNode& node)->bool {
      if ( node.is_ppo() ) {
	mOutputMark[node.id()] = true;
      }
      if ( mOutputMark[node.id()] ) {
	mOutputList.push_back(node);
	return false;
      }
      return true;
    }
  );
}

// @brief デストラクタ
BoolDiffEnc::~BoolDiffEnc()
{
}

// @brief データ構造の初期化を行う．
void
BoolDiffEnc::init()
{
  if ( network().has_prev_state() ) {
    engine().add_prev_node(mRoot);
  }

  mFvarMap.init(network().node_num());
  mDvarMap.init(network().node_num());
  mPropVarList.resize(output_num());
}

// @brief 必要な変数を割り当て CNF 式を作る．
void
BoolDiffEnc::make_cnf()
{
  // fvar/dvar の割り当て
  for ( auto node: mTfoList ) {
    // まずファンインのノードの変数として gvar をデフォルト値
    // として設定しておく．
    for ( auto inode: node.fanin_list() ) {
      auto glit = gvar(inode);
      mFvarMap.set_vid(inode, glit);
    }
  }
  for ( auto node: mTfoList ) {
    auto flit = solver().new_variable(true);
    auto dlit = solver().new_variable(false);
    mFvarMap.set_vid(node, flit);
    mDvarMap.set_vid(node, dlit);
  }

  // CNF の生成
  GateEnc fval_enc(solver(), mFvarMap);
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      fval_enc.make_cnf(node);
    }
    make_dchain_cnf(node);
  }

  { // mRoot には故障の影響が伝搬している．
    auto glit = gvar(mRoot);
    auto flit = fvar(mRoot);
    solver().add_clause( glit,  flit);
    solver().add_clause(~glit, ~flit);
  }

  // 各出力の微分結果を表す変数を作る．
  for ( SizeType i = 0; i < output_num(); ++ i ) {
    auto node = mOutputList[i];
    auto plit = solver().new_variable(true);
    auto dlit = dvar(node);
    solver().add_buffgate(plit, dlit);
    mPropVarList[i] = plit;
  }

  // 微分結果を表す変数を作る．
  if ( output_num() > 1 ) {
    std::vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(output_num());
    for ( auto plit: mPropVarList ) {
      tmp_lits.push_back(plit);
    }
    mPropVar = solver().new_variable(true);
    solver().add_orgate(mPropVar, tmp_lits);
  }
  else {
    // 1出力ならその出力の PropVar が伝搬条件そのもの
    mPropVar = mPropVarList[0];
  }
}

// @brief 関連するノードのリストを返す．
const TpgNodeList&
BoolDiffEnc::node_list() const
{
  return mTfoList;
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
BoolDiffEnc::make_dchain_cnf(
  const TpgNode& node
)
{
  auto glit = gvar(node);
  auto flit = fvar(node);
  auto dlit = dvar(node);

  {
    if ( !glit.is_valid() ) {
      throw std::logic_error{"glit is invalid"};
    }
    if ( !flit.is_valid() ) {
      throw std::logic_error{"flit is invalid"};
    }
    if ( !dlit.is_valid() ) {
      throw std::logic_error{"dlit is invalid"};
    }
  }

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  solver().add_clause(~glit, ~flit, ~dlit);
  solver().add_clause( glit,  flit, ~dlit);

  if ( mOutputMark[node.id()] ) {
    solver().add_clause(~glit,  flit,  dlit);
    solver().add_clause( glit, ~flit,  dlit);
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1
    SizeType nfo = node.fanout_num();
    if ( nfo == 1 ) {
      auto odlit = mDvarMap(node.fanout(0));
      solver().add_clause(~dlit, odlit);
    }
    else {
      std::vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node.fanout_list() ) {
	tmp_lits.push_back(mDvarMap(onode));
      }

      tmp_lits.push_back(~dlit);
      solver().add_clause(tmp_lits);

      auto imm_dom = node.imm_dom();
      if ( imm_dom.is_valid() ) {
	auto odlit = mDvarMap(imm_dom);
	solver().add_clause(~dlit, odlit);
      }
    }
  }
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
BoolDiffEnc::extract_sufficient_condition(
  const SatModel& model,
  const AssignList& assign_list
)
{
  return (*mExtractor)(root_node(),
		       engine().gvar_map(),
		       mFvarMap,
		       model,
		       assign_list);
}

// @brief SAT問題の解から十分条件を求める．
SuffCond
BoolDiffEnc::extract_sufficient_condition(
  SizeType pos,
  const SatModel& model,
  const AssignList& assign_list
)
{
  if ( pos >= output_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return (*mExtractor)(root_node(),
		       engine().gvar_map(),
		       mFvarMap,
		       mOutputList[pos],
		       model,
		       assign_list);
}

END_NAMESPACE_DRUID
