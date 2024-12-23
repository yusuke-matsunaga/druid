#ifndef CNFGENIMPL2_H
#define CNFGENIMPL2_H

/// @file CnfGenImpl2.h
/// @brief CnfGenImpl2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "ym/Expr.h"
#include "ym/Bdd.h"
#include "ym/BddMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenImpl2 CnfGenImpl2.h "CnfGenImpl2.h"
/// @brief CnfGen の下請けクラス
//////////////////////////////////////////////////////////////////////
class CnfGenImpl2
{
public:

  /// @brief コンストラクタ
  explicit
  CnfGenImpl2(
    StructEngine& engine ///< [in] StructEngine
  ) : mEngine{engine}
  {
  }

  /// @brief デストラクタ
  ~CnfGenImpl2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 式を CNF に変換する．
  void
  make_cnf(
    const Expr& expr,               ///< [in] 式
    vector<SatLiteral>& assumptions ///< [out] 活性化条件を追加する．
  );

  /// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
  void
  calc_cnf_size(
    const Expr& expr
  );

  /// @brief calc_cnf_size() の結果を返す．
  CnfSize
  cnf_size() const
  {
    return mCnfSize;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を BDD に変換する．
  Bdd
  conv_to_bdd(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief BDD を CNF 式に変換する．
  void
  bdd_to_cnf(
    const Bdd& bdd,
    vector<SatLiteral>& lit_list
  );

  /// @brief BDD を CNF に変換した際の項数とリテラル数を計算する．
  SizeType
  calc_cnf_size(
    const Bdd& bdd
  );

  /// @brief BDDの変数を SAT ソルバのリテラルに変換する．
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

  // BDD マネージャ
  BddMgr mBddMgr;

  // 論理式の変数番号と BDD の変数の対応関係を保持する辞書
  std::unordered_map<SizeType, BddVar> mVarDict;

  // BDD と対応する SATリテラルを保持する辞書
  std::unordered_map<Bdd, vector<SatLiteral>> mResultDict;

  // サイズ計算用の辞書
  std::unordered_map<Bdd, SizeType> mSizeDict;

  // calc_cnf_size の結果
  CnfSize mCnfSize{0, 0};

};

END_NAMESPACE_DRUID

#endif // CNFGENIMPL_H
