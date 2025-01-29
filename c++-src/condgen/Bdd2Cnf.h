#ifndef BDD2CNF_H
#define BDD2CNF_H

/// @file Bdd2Cnf.h
/// @brief Bdd2Cnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "ym/Bdd.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Bdd2Cnf Bdd2Cnf.h "Bdd2Cnf.h"
/// @brief BDD を CNF に変換するクラス
//////////////////////////////////////////////////////////////////////
class Bdd2Cnf
{
public:

  /// @brief コンストラクタ
  Bdd2Cnf(
    StructEngine& engine
  ) : mEngine{engine}
  {
  }

  /// @brief デストラクタ
  ~Bdd2Cnf() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief BDD を CNF に変換する．
  SatLiteral
  conv_to_cnf(
    const Bdd& bdd ///< [in] BDD
  );

  /// @brief BDD を CNF に変換した時のサイズを見積もる．
  static
  CnfSize
  calc_cnf_size(
    const Bdd& bdd ///< [in] BDD
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  // @brief Bdd 変数を SAT ソルバのリテラルに変換する．
  SatLiteral
  conv_to_literal(
    const BddVar& var
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // StructEngine
  StructEngine& mEngine;

  // 結果を記録しておく辞書
  std::unordered_map<Bdd, SatLiteral> mResultDict;

};

END_NAMESPACE_DRUID

#endif // BDD2CNF_H
