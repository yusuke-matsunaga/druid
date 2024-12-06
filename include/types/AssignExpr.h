#ifndef ASSIGNEXPR_H
#define ASSIGNEXPR_H

/// @file AssignExpr.h
/// @brief AssignExpr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"
#include "AssignMap.h"
#include "ym/Expr.h"
#include "ym/Literal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AssignExpr AssignExpr.h "AssignExpr.h"
/// @brief Assign を用いた論理式を表すクラス
///
/// 実際には Assign と変数番号の割り当てと Expr を持つクラス
//////////////////////////////////////////////////////////////////////
class AssignExpr
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// - 定数0を表す．
  AssignExpr(
  ) : mExpr{Expr::zero()}
  {
  }

  /// @brief コンストラクタ
  AssignExpr(
    const Expr& expr,                 ///< [in] 論理式
    const vector<Assign>& assign_list ///< [in] 割り当てのリスト
  ) : mExpr{expr},
      mMap{assign_list}
  {
  }

  /// @brief AssignList からの変換コンストラクタ
  ///
  /// AssignList をキューブだと見なす．
  explicit
  AssignExpr(
    const AssignList& cube ///< [in] キューブ
  );

  /// @brief デストラクタ
  ~AssignExpr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を返す．
  Expr
  expr() const
  {
    return mExpr;
  }

  /// @brief 正規化した論理式を返す．
  ///
  /// 変数番号を ノード番号 x 2 + 時刻(0 or 1) に
  /// 置き換える．
  Expr
  normalize() const;

  /// @brief 変数の数を返す．
  SizeType
  variable_num() const
  {
    return mMap.variable_num();
  }

  /// @brief 変数番号に対応する割り当てを返す．
  Assign
  assign(
    SizeType var ///< [in] 変数番号 ( 0 <= var < variable_num() )
  ) const
  {
    return mMap.assign(var);
  }

  /// @brief リテラルに対応する割り当てを返す．
  Assign
  assign(
    Literal lit ///< [in] リテラル
  ) const
  {
    return mMap.assign(lit);
  }

  /// @brief 割り当てマップを返す．
  const AssignMap&
  assign_map() const
  {
    return mMap;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 変数番号をキーとして対応する Assign を持つリスト
  AssignMap mMap;

};

END_NAMESPACE_DRUID

#endif // ASSIGNEXPR_H
