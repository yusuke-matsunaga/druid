#ifndef TESTCOND_H
#define TESTCOND_H

/// @file TestCond.h
/// @brief TestCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestCond TestCond.h "TestCond.h"
/// @brief 故障検出条件を表すクラス
///
/// 条件自体は「式」(Expr)で表されるが，式中の変数番号に対する割り当て
/// の情報を持つ．
//////////////////////////////////////////////////////////////////////
class TestCond
{
public:

  /// @brief 空のコンストラクタ
  TestCond() :
    mExpr{Expr::zero()}
  {
  }

  /// @brief 内容を指定したコンストラクタ
  TestCond(
    const Expr& expr,                 ///< [in] 論理式
    const vector<Assign>& assign_list ///< [in] 割り当てのリスト
  ) : mExpr{expr},
      mAssignList{assign_list}
  {
  }

  /// @brief デストラクタ
  ~TestCond() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 式を返す．
  Expr
  expr() const
  {
    return mExpr;
  }

  /// @brief 変数の数を返す．
  SizeType
  variable_num() const
  {
    return mAssignList.size();
  }

  /// @brief 変数に対応する割り当てを返す．
  Assign
  get_assign(
    SizeType var ///< [in] 変数番号 ( 0 <= var < variable_num() )
  ) const
  {
    if ( var >= variable_num() ) {
      throw std::out_of_range{"var is out of range"};
    }
    return mAssignList[var];
  }

  /// @brief 変数番号順の割り当てリストを返す．
  const vector<Assign>&
  assign_list() const
  {
    return mAssignList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 割り当てのリスト
  vector<Assign> mAssignList;

};

END_NAMESPACE_DRUID

#endif // TESTCOND_H
