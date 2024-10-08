
/// @file Extractor2.cc
/// @brief Extractor2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor2.h"
#include "PropData.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = 0;


END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor2
//////////////////////////////////////////////////////////////////////

// @brief 値割り当てを１つ求める．
AssignExpr
Extractor2::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  PropData data{root, gvar_map, fvar_map, model};

  vector<AssignExpr> assign_list;
  for ( auto& boundary_data: data.boundary_data_list() ) {
    vector<AssignExpr> and_list;
    for ( auto node: boundary_data.mNodeList ) {
      auto& assign_data = data.assign_data(node);
      if ( assign_data.mAndCond ) {
	for ( auto& assign: assign_data.mAssignList ) {
	  and_list.push_back(AssignExpr::make_literal(assign));
	}
      }
      else {
	// cnode_list のどれか１つを満たす必要がある．
	vector<AssignExpr> or_list;
	for ( auto& assign: assign_data.mAssignList ) {
	  or_list.push_back(AssignExpr::make_literal(assign));
	}
	auto expr = AssignExpr::make_or(or_list);
	and_list.push_back(expr);
      }
    }
    auto expr = AssignExpr::make_and(and_list);
    assign_list.push_back(expr);
  }
  return AssignExpr::make_or(assign_list);
}

END_NAMESPACE_DRUID
