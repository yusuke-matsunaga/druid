#ifndef EXPR2CNF_H
#define EXPR2CNF_H

/// @file Expr2Cnf.h
/// @brief Expr2Cnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Expr.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Expr2Cnf Expr2Cnf.h "Expr2Cnf.h"
/// @brief Expr を CNF 式に変換するためのクラス
//////////////////////////////////////////////////////////////////////
class Expr2Cnf
{
public:

  Expr2Cnf(
    SatSolver& solver,
    const std::unordered_map<SizeType, SatLiteral>& lit_map
  ) : mSolver{solver},
      mLitMap{lit_map}
  {
  }

  ~Expr2Cnf() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr を CNF 式に変換する．
  vector<SatLiteral>
  make_cnf(
    const Expr& expr
  );

  /// @brief Expr を CNF 式に変換したときのサイズを計算する．
  static
  CnfSize
  calc_cnf_size(
    const Expr& expr
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

  // 変数番号をキーにして SatLiteral を記憶する辞書
  const std::unordered_map<SizeType, SatLiteral>& mLitMap;

};

END_NAMESPACE_DRUID

#endif // EXPR2CNF_H
