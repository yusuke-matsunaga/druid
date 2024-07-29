
/// @file AssignExpr.cc
/// @brief AssignExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "AssignExpr.h"
#include "AssignList.h"


BEGIN_NAMESPACE_DRUID

// @brief 論理積を作る．
AssignExpr
AssignExpr::make_and(
  const vector<AssignExpr>& opr_list
)
{
  ASSERT_COND( !opr_list.empty() );
  vector<AssignExpr> child_list;
  for ( auto& opr: opr_list ) {
    if ( opr.is_and() ) {
      auto& opr_list1 = opr.opr_list();
      child_list.insert(child_list.end(),
			opr_list1.begin(),
			opr_list1.end());
    }
    else {
      child_list.push_back(opr);
    }
  }
  return AssignExpr{2, child_list};
}

// @brief 論理積を作る．
AssignExpr
AssignExpr::make_and(
  const AssignList& opr_list
)
{
  ASSERT_COND( opr_list.size() > 0 );
  vector<AssignExpr> child_list;
  child_list.reserve(opr_list.size());
  for ( auto nv: opr_list ) {
    child_list.push_back(AssignExpr::make_literal(nv));
  }
  return AssignExpr{1, child_list};
}

// @brief 論理和を作る．
AssignExpr
AssignExpr::make_or(
  const vector<AssignExpr>& opr_list
)
{
  ASSERT_COND( !opr_list.empty() );
  vector<AssignExpr> child_list;
  for ( auto& opr: opr_list ) {
    if ( opr.is_or() ) {
      auto& opr_list1 = opr.opr_list();
      child_list.insert(child_list.end(),
			opr_list1.begin(),
			opr_list1.end());
    }
    else {
      child_list.push_back(opr);
    }
  }
  return AssignExpr{3, child_list};
}

// @brief 含まれるキューブを取り出す．
AssignList
AssignExpr::extract_cube() const
{
}

// @brief リテラル数を返す．
SizeType
AssignExpr::literal_num() const
{
}

// @brief AND演算子
AssignExpr
AssignExpr::operator&(
  const AssignExpr& right
) const
{
}

// @brief AND演算付き代入
AssignExpr&
AssignExpr::operator&=(
  const AssignExpr& right
)
{
}

END_NAMESPACE_DRUID
