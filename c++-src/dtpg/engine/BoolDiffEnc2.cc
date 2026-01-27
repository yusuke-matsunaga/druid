
/// @file BoolDiffEnc2.cc
/// @brief BoolDiffEnc2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BoolDiffEnc2.h"
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
BoolDiffEnc2::BoolDiffEnc2(
  StructEngine& engine,
  const TpgNode* root,
  const JsonValue& option
) : SubEnc(engine),
    mRoot{root},
    mExtractor{Extractor::new_impl(get_option(option, "extractor"))}
{
  std::unordered_set<SizeType> tfo_mark;
  mTfoList = TpgNodeSet::get_tfo_list(
    engine.network().node_num(), mRoot,
    [&](const TpgNode* node) {
      tfo_mark.emplace(node->id());
      if ( node->is_ppo() ) {
	mOutputList.push_back(node);
      }
    }
  );
  auto n = mOutputList.size();
  mPartialTfoList.resize(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    std::unordered_set<SizeType> dfs_mark;
    dfs(mOutputList[i], tfo_mark, dfs_mark);
    mPartialTfoList[i].reserve(dfs_mark.size());
    for ( auto node: mTfoList ) {
      mPartialTfoList[i].push_back(node);
    }
  }

  mFvarMapList.resize(n);
  mDvarMapList.resize(n);
  mPropVarList.resize(n);
  auto nn = engine.network().node_num();
  for ( SizeType i = 0; i < n; ++ i ) {
    mFvarMapList[i].init(nn);
    mDvarMapList[i].init(nn);
  }
}

// @brief デストラクタ
BoolDiffEnc2::~BoolDiffEnc2()
{
}

// @brief 必要な変数を割り当て CNF 式を作る．
void
BoolDiffEnc2::make_cnf()
{
  // fvar/dvar の割り当て
  for ( auto node: mTfoList ) {
    // まずファンインのノードの変数として gvar をデフォルト値
    // として設定しておく．
    for ( auto inode: node->fanin_list() ) {
      auto glit = gvar(inode);
      for ( auto& vidmap: mFvarMapList ) {
	vidmap.set_vid(inode, glit);
      }
    }
  }
  for ( SizeType i = 0; i < output_num(); ++ i ) {
    for ( auto node: mPartialTfoList[i] ) {
      auto flit = solver().new_variable(true);
      auto dlit = solver().new_variable(false);
      mFvarMapList[i].set_vid(node, flit);
      mDvarMapList[i].set_vid(node, dlit);
    }

    // CNF の生成
    GateEnc fval_enc(solver(), mFvarMapList[i]);
    for ( auto node: mPartialTfoList[i] ) {
      if ( node != mRoot ) {
	fval_enc.make_cnf(node);
      }
      make_dchain_cnf(node, i);
    }

    { // mRoot には故障の影響が伝搬している．
      auto glit = gvar(mRoot);
      auto flit = fvar(mRoot, i);
      solver().add_clause(~glit, ~flit);
      solver().add_clause( glit,  flit);
    }

    // 微分結果を表す変数を作る．
    auto plit = solver().new_variable(true);
    mPropVarList[i] = plit;
    auto dlit = dvar(mOutputList[i], i);
    solver().add_buffgate(plit, dlit);
  }
}

// @brief 関連するノードのリストを返す．
const vector<const TpgNode*>&
BoolDiffEnc2::node_list() const
{
  return mTfoList;
}

// @brief 故障伝搬条件を表すCNF式を生成する．
void
BoolDiffEnc2::make_dchain_cnf(
  const TpgNode* node,
  SizeType pos
)
{
  auto glit = gvar(node);
  auto flit = fvar(node, pos);
  auto dlit = dvar(node, pos);

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
      auto odlit = dvar(node->fanout(0), pos);
      solver().add_clause(~dlit, odlit);
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	tmp_lits.push_back(dvar(onode, pos));
      }

      tmp_lits.push_back(~dlit);
      solver().add_clause(tmp_lits);

      auto imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	auto odlit = dvar(imm_dom, pos);
	solver().add_clause(~dlit, odlit);
      }
    }
  }
}

// @brief 直前の check() が成功したときの十分条件を求める．
AssignList
BoolDiffEnc2::extract_sufficient_condition(
  SizeType pos
)
{
  if ( pos >= output_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return (*mExtractor)(root_node(),
		       engine().gvar_map(),
		       mFvarMapList[pos],
		       solver().model());
}

END_NAMESPACE_DRUID
