
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

END_NONAMESPACE

// @brief コンストラクタ
BoolDiffEnc2::BoolDiffEnc2(
  StructEngine& engine,
  const TpgNode* root,
  const JsonValue& option
) : SubEnc{engine},
    mRoot{root},
    mF0varMap{engine.network().node_num()},
    mF1varMap{engine.network().node_num()},
    mDvarMap{engine.network().node_num()},
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
      mF0varMap.set_vid(inode, glit);
      mF1varMap.set_vid(inode, glit);
    }
  }
  for ( auto node: mTfoList ) {
    auto f0lit = solver().new_variable(true);
    auto f1lit = solver().new_variable(true);
    auto dlit = solver().new_variable(false);
    mF0varMap.set_vid(node, f0lit);
    mF1varMap.set_vid(node, f1lit);
    mDvarMap.set_vid(node, dlit);
  }

  // CNF の生成
  GateEnc f0val_enc{solver(), mF0varMap};
  GateEnc f1val_enc{solver(), mF1varMap};
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
      f0val_enc.make_cnf(node);
      f1val_enc.make_cnf(node);
    }
    make_dchain_cnf(node);
  }

  { // mRoot には故障の影響が伝搬している．
    auto f0lit = f0var(mRoot);
    auto f1lit = f1var(mRoot);
    solver().add_clause(~f0lit);
    solver().add_clause( f1lit);
  }

  // 微分結果を表す変数を作る．
  ASSERT_COND( !mOutputList.empty() );
  if ( mOutputList.size() > 1 ) {
    mPropVar = solver().new_variable(true);
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(mOutputList.size());
    for ( auto node: mOutputList ) {
      auto dlit = dvar(node);
      tmp_lits.push_back(dlit);
    }
    solver().add_orgate(mPropVar, tmp_lits);
  }
  else {
    // 1出力ならその出力の dlit が伝搬条件そのもの
    mPropVar = dvar(mOutputList[0]);
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
  const TpgNode* node
)
{
  auto f0lit = f0var(node);
  auto f1lit = f1var(node);
  auto dlit = dvar(node);

  // dlit -> XOR(f0lit, f1lit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  solver().add_clause(~f0lit, ~f1lit, ~dlit);
  solver().add_clause( f0lit,  f1lit, ~dlit);

  if ( node->is_ppo() ) {
    solver().add_clause(~f0lit,  f1lit,  dlit);
    solver().add_clause( f0lit, ~f1lit,  dlit);
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
BoolDiffEnc2::extract_sufficient_condition()
{
  return (*mExtractor)(root_node(),
		       mF0varMap,
		       mF1varMap,
		       solver().model());
}

// @brief 直前の check() が成功したときの十分条件を求める．
AssignExpr
BoolDiffEnc2::extract_sufficient_conditions()
{
  return Extractor::extract_all(root_node(),
				mF0varMap,
				mF1varMap,
				solver().model());
}

END_NAMESPACE_DRUID
