
/// @file AssignExpr.cc
/// @brief AssignExpr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "AssignExpr.h"
#include "AssignList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// AssignExpr のリストを正規化する．
void
normalize(
  vector<AssignExpr>& src_list
)
{
  sort(src_list.begin(), src_list.end());

  SizeType n = src_list.size();
  SizeType wpos = 0;
  for ( SizeType rpos = 0; rpos < n; ++ rpos ) {
    auto& expr = src_list[rpos];
    if ( wpos == 0 || (wpos > 0 && src_list[wpos - 1] != expr) ) {
      if ( wpos != rpos ) {
	src_list[wpos] = expr;
      }
      ++ wpos;
    }
  }
  if ( wpos < n ) {
    src_list.erase(src_list.begin() + wpos, src_list.end());
  }
}

END_NONAMESPACE

// @brief 論理積を作る．
AssignExpr
AssignExpr::make_and(
  vector<AssignExpr>& opr_list
)
{
  ASSERT_COND( !opr_list.empty() );
  normalize(opr_list);
  if ( opr_list.size() == 1 ) {
    return opr_list.front();
  }
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
  SizeType n = opr_list.size();
  vector<AssignExpr> tmp_list(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto assign = opr_list.elem(i);
    tmp_list[i] = make_literal(assign);
  }
  return make_and(tmp_list);
}

// @brief 論理和を作る．
AssignExpr
AssignExpr::make_or(
  vector<AssignExpr>& opr_list
)
{
  ASSERT_COND( !opr_list.empty() );
  normalize(opr_list);
  if ( opr_list.size() == 1 ) {
    return opr_list.front();
  }
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

// @brief リテラル数を返す．
SizeType
AssignExpr::literal_num() const
{
  if ( is_literal() ) {
    return 1;
  }
  SizeType lit_num = 0;
  for ( auto& expr: opr_list() ) {
    lit_num += expr.literal_num();
  }
  return lit_num;
}

// @brief AND演算子
AssignExpr
AssignExpr::operator&(
  const AssignExpr& right
) const
{
  vector<AssignExpr> tmp_list;
  SizeType n = 0;
  if ( is_and() ) {
    n += opr_list().size();
  }
  else {
    n += 1;
  }
  if ( right.is_and() ) {
    n += right.opr_list().size();
  }
  else {
    n += 1;
  }
  tmp_list.reserve(n);
  if ( is_and() ) {
    for ( auto& expr: opr_list() ) {
      tmp_list.push_back(expr);
    }
  }
  else {
    tmp_list.push_back(*this);
  }
  if ( right.is_and() ) {
    for ( auto& expr: right.opr_list() ) {
      tmp_list.push_back(expr);
    }
  }
  else {
    tmp_list.push_back(right);
  }
  return make_and(tmp_list);
}

// @brief 等価関係の比較関数
bool
operator==(
  const AssignExpr& left,
  const AssignExpr& right
)
{
  if ( left.mType != right.mType ) {
    return false;
  }
  if ( left.is_literal() ) {
    return left.mLiteral == right.mLiteral;
  }
  SizeType n = left.mOprList.size();
  if ( right.mOprList.size() != n ) {
    return false;
  }
  for ( SizeType i = 0; i < n; ++ i ) {
    if ( left.mOprList[i] != right.mOprList[i] ) {
      return false;
    }
  }
  return true;
}

// @brief 大小関係の比較関数
bool
operator<(
  const AssignExpr& left,
  const AssignExpr& right
)
{
  if ( left.mType < right.mType ) {
    return true;
  }
  if ( left.mType > right.mType ) {
    return false;
  }
  if ( left.is_literal() ) {
    return left.mLiteral < right.mLiteral;
  }
  SizeType n1 = left.mOprList.size();
  SizeType n2 = right.mOprList.size();
  SizeType n = std::min(n1, n2);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& expr1 = left.mOprList[i];
    auto& expr2 = right.mOprList[i];
    if ( expr1 < expr2 ) {
      return true;
    }
    if ( expr1 > expr2 ) {
      return false;
    }
  }
  if ( n == n1 ) {
    return true;
  }
  return false;
}

// @brief AssignExpr のストリーム出力演算子
ostream&
operator<<(
  ostream& s,
  const AssignExpr& expr
)
{
  if ( expr.is_literal() ) {
    auto assign = expr.literal();
    s << assign;
  }
  else if ( expr.is_and() ) {
    auto& opr_list = expr.opr_list();
    s << "(";
    const char* op = "";
    for ( auto& expr: opr_list ) {
      s << op << expr;
      op = " & ";
    }
    s << ")";
  }
  else if ( expr.is_or() ) {
    auto& opr_list = expr.opr_list();
    s << "(";
    const char* op = "";
    for ( auto& expr: opr_list ) {
      s << op << expr;
      op = " | ";
    }
    s << ")";
  }
  return s;
}

END_NAMESPACE_DRUID
