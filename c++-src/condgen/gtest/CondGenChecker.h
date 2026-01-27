#ifndef CONDGENCHECKER_H
#define CONDGENCHECKER_H

/// @file CondGenChecker.h
/// @brief CondGenChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/BdEngine.h"
#include "condgen/DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenChecker CondGenChecker.h "CondGenChecker.h"
/// @brief CondGen の結果の検証を行うクラス
//////////////////////////////////////////////////////////////////////
class CondGenChecker
{
public:

  /// @brief コンストラクタ
  CondGenChecker(
    const TpgNetwork& network,
    const DetCond& cond
  );

  /// @brief デストラクタ
  ~CondGenChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief エンジンを返す．
  BdEngine&
  engine()
  {
    return mEngine;
  }

  /// @brief SAT ソルバを返す．
  SatSolver&
  solver()
  {
    return mEngine.solver();
  }

  /// @brief DetCond の条件を表すリテラルを返す．
  SatLiteral
  lit1() const
  {
    return mLit1;
  }

  /// @brief BoolDiffEnc の条件を表すリテラルを返す．
  SatLiteral
  lit2() const
  {
    return mLit2;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 式中のリテラルを SAT リテラルに変換する．
  SatLiteral
  conv_to_literal(
    Literal src_lit
  );

  /// @brief 式中のリテラルのリストを SAT リテラルのリストに変換する．
  std::vector<SatLiteral>
  conv_to_literals(
    const std::vector<Literal>& src_lits
  );

  /// @brief DetCond::CondData を CNF に変換する．
  SatLiteral
  make_cnf(
    const DetCond::CondData& data
  );

  /// @brief DetCond を CNF に変換する．
  SatLiteral
  make_cnf();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BoolDiffEngine
  BdEngine mEngine;

  // 対象の条件
  const DetCond& mCond;

  // DetCond の条件を表すリテラル
  SatLiteral mLit1;

  // BoolDiffEnc の条件を表すリテラル
  SatLiteral mLit2;

};

END_NAMESPACE_DRUID

#endif // CONDGENCHECKER_H
