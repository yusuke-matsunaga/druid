
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
  BaseEnc& base_enc,
  const TpgNode* root,
  const JsonValue& option
) : SubEnc{base_enc},
    mRoot{root},
    mFvarMap{base_enc.network().node_num()},
    mDvarMap{base_enc.network().node_num()},
    mExtractor{get_option(option, "extractor")}
{
  mTfoList = TpgNodeSet::get_tfo_list(
    base_enc.network().node_num(), mRoot,
    [&](const TpgNode* node) {
      if ( node->is_ppo() ) {
	mOutputList.push_back(node);
      }
    }
  );
}

// @brief 必要な変数を割り当て CNF 式を作る．
void
BoolDiffEnc::make_cnf()
{
  // fvar/dvar の割り当て
  for ( auto node: base_enc().cur_node_list() ) {
    for ( auto inode: node->fanin_list() ) {
      // まず gvar をデフォルト値として設定しておく．
      // 実際に使われるのは TfoList の境界部分のノードだけ
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
  GateEnc fval_enc{solver(), mFvarMap};
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
NodeTimeValList
BoolDiffEnc::extract_sufficient_condition()
{
  return mExtractor(
    root_node(),
    base_enc().gvar_map(),
    mFvarMap,
    solver().model()
  );
}

END_NAMESPACE_DRUID
