
/// @file BoolDiffEnc.cc
/// @brief BoolDiffEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BoolDiffEnc.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"
#include "GateEnc.h"
#include "Extractor.h"


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
BoolDiffEnc::BoolDiffEnc(
  StructEngine& engine,
  const TpgNode* root,
  const JsonValue& option
) : SubEnc(engine),
    mRoot{root},
    mFvarMap(engine.network().node_num()),
    mDvarMap(engine.network().node_num()),
    mExtractor{Extractor::new_impl(get_option(option, "extractor"))}
{
  mTfoList = TpgNodeSet::get_tfo_list(
    engine.network().node_num(), mRoot,
    [&](const TpgNode* node) {
      if ( node->is_ppo() ) {
	mOutputList.push_back(node);
      }
    }
  );
  mPropVarList.resize(output_num());
}

BEGIN_NONAMESPACE

void
dfs(
  const TpgNode* node,
  const std::unordered_set<SizeType>& tfo_mark,
  std::unordered_set<SizeType>& dfs_mark
)
{
  if ( tfo_mark.count(node->id()) == 0 ) {
    return;
  }
  if ( dfs_mark.count(node->id()) > 0 ) {
    return;
  }
  dfs_mark.emplace(node->id());
  for ( auto inode: node->fanin_list() ) {
    dfs(inode, tfo_mark, dfs_mark);
  }
}

END_NONAMESPACE

// @brief コンストラクタ
BoolDiffEnc::BoolDiffEnc(
  StructEngine& engine,
  const TpgNode* root,
  const vector<const TpgNode*>& output_list,
  const JsonValue& option
) : SubEnc(engine),
    mRoot{root},
    mOutputList{output_list},
    mFvarMap(engine.network().node_num()),
    mDvarMap(engine.network().node_num()),
    mExtractor{Extractor::new_impl(get_option(option, "extractor"))}
{
  std::unordered_set<SizeType> tfo_mark;
  auto tfo_list = TpgNodeSet::get_tfo_list(
    engine.network().node_num(), mRoot,
    [&](const TpgNode* node) {
      tfo_mark.emplace(node->id());
    }
  );
  std::unordered_set<SizeType> dfs_mark;
  for ( auto output: mOutputList ) {
    dfs(output, tfo_mark, dfs_mark);
  }
  mTfoList.reserve(dfs_mark.size());
  for ( auto node: tfo_list ) {
    if ( dfs_mark.count(node->id()) > 0 ) {
      mTfoList.push_back(node);
    }
  }
  mPropVarList.resize(output_num());
}

// @brief デストラクタ
BoolDiffEnc::~BoolDiffEnc()
{
}

// @brief 必要な変数を割り当て CNF 式を作る．
void
BoolDiffEnc::make_cnf()
{
  // fvar/dvar の割り当て
  for ( auto node: mTfoList ) {
    // まずファンインのノードの変数として gvar をデフォルト値
    // として設定しておく．
    for ( auto inode: node->fanin_list() ) {
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
  mPropVarList.resize(output_num());
  for ( SizeType i = 0; i < output_num(); ++ i ) {
    auto node = mOutputList[i];
    auto plit = solver().new_variable(true);
    auto dlit = dvar(node);
    solver().add_buffgate(plit, dlit);
    mPropVarList[i] = plit;
  }

  // 微分結果を表す変数を作る．
  if ( mOutputList.size() > 1 ) {
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(output_num());
    for ( auto plit: mPropVarList ) {
      tmp_lits.push_back(plit);
    }
    auto plit = solver().new_variable(true);
    solver().add_orgate(plit, tmp_lits);
    mPropVar = plit;
  }
  else {
    // 1出力ならその出力の PropVar が伝搬条件そのもの
    mPropVar = mPropVarList[0];
  }
}

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
BoolDiffEnc::node_list() const
{
  return mTfoList;
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
BoolDiffEnc::make_dchain_cnf(
  const TpgNode* node
)
{
  auto glit = gvar(node);
  auto flit = fvar(node);
  auto dlit = dvar(node);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  solver().add_clause(~glit, ~flit, ~dlit);
  solver().add_clause( glit,  flit, ~dlit);

  if ( node->is_ppo() ) {
    solver().add_clause(~glit,  flit,  dlit);
    solver().add_clause( glit, ~flit,  dlit);
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1
    SizeType nfo = node->fanout_num();
    if ( nfo == 1 ) {
      auto odlit = mDvarMap(node->fanout(0));
      solver().add_clause(~dlit, odlit);
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	tmp_lits.push_back(mDvarMap(onode));
      }

      tmp_lits.push_back(~dlit);
      solver().add_clause(tmp_lits);

      auto imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	auto odlit = mDvarMap(imm_dom);
	solver().add_clause(~dlit, odlit);
      }
    }
  }
}

// @brief 直前の check() が成功したときの十分条件を求める．
AssignList
BoolDiffEnc::extract_sufficient_condition()
{
  return (*mExtractor)(root_node(),
		       engine().gvar_map(),
		       mFvarMap,
		       solver().model());
}

// @brief 直前の check() が成功したときの十分条件を求める．
AssignList
BoolDiffEnc::extract_sufficient_condition(
  SizeType pos
)
{
  if ( pos >= output_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return (*mExtractor)(root_node(),
		       engine().gvar_map(),
		       mFvarMap,
		       mOutputList[pos],
		       solver().model());
}

// @brief 直前の check() が成功したときの十分条件を求める．
AssignExpr
BoolDiffEnc::extract_sufficient_conditions()
{
  return Extractor::extract_all(root_node(),
				engine().gvar_map(),
				mFvarMap,
				solver().model());
}

// @brief CNF 式を作成する際のサイズを見積もる．
CnfSize
BoolDiffEnc::calc_cnf_size()
{
  auto size = CnfSize::zero();

  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      size += GateEnc::calc_cnf_size(node);
    }
    size += calc_dchain_cnf_size(node);
  }

  { // mRoot には故障の影響が伝搬している．
    // solver().add_clause( glit,  flit);
    // solver().add_clause(~glit, ~flit);
    size += CnfSize{2, 4};
  }

  // 微分結果を表す変数を作る．
  SizeType n = mOutputList.size();
  if ( n > 1 ) {
    size += CnfSize{n + 1, n * 2 + n + 1};
  }
  return size;
}

// @brief 故障伝搬条件を表すCNF式のサイズを見積もる．
CnfSize
BoolDiffEnc::calc_dchain_cnf_size(
  const TpgNode* node
)
{
  auto size = CnfSize::zero();

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  // solver().add_clause(~glit, ~flit, ~dlit);
  // solver().add_clause( glit,  flit, ~dlit);
  size += CnfSize{2, 6};

  if ( node->is_ppo() ) {
    // solver().add_clause(~glit,  flit,  dlit);
    // solver().add_clause( glit, ~flit,  dlit);
    size += CnfSize{2, 6};
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1
    SizeType nfo = node->fanout_num();
    if ( nfo == 1 ) {
      // solver().add_clause(~dlit, odlit);
      size += CnfSize{1, 2};
    }
    else {
      // solver().add_clause(tmp_lits);
      size += CnfSize{1, nfo + 1};

      auto imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	// solver().add_clause(~dlit, odlit);
	size += CnfSize{1, 2};
      }
    }
  }

  return size;
}

END_NAMESPACE_DRUID
