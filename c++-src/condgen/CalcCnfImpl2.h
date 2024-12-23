#ifndef CALCCNFIMPL2_H
#define CALCCNFIMPL2_H

/// @file CalcCnfImpl2.h
/// @brief CalcCnfImpl2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Expr.h"
#include "ym/CnfSize.h"
#include "ym/Bdd.h"
#include "ym/BddMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CalcCnfImpl2 CalcCnfImpl2.h "CalcCnfImpl2.h"
/// @brief CalcCnf の下請けクラス
//////////////////////////////////////////////////////////////////////
class CalcCnfImpl2
{
public:

  /// @brief コンストラクタ
  CalcCnfImpl2() = default;

  /// @brief デストラクタ
  ~CalcCnfImpl2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 式を CNF に変換した際の項数とリテラル数を計算する．
  void
  run(
    const Expr& expr
  );

  /// @brief run() の結果を返す．
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

  /// @brief BDD を CNF に変換した際の項数とリテラル数を計算する．
  SizeType
  calc_cnf_size(
    const Bdd& bdd
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BDD マネージャ
  BddMgr mBddMgr;

  // サイズ計算用の辞書
  std::unordered_map<Bdd, SizeType> mSizeDict;

  // calc_cnf_size の結果
  CnfSize mCnfSize{0, 0};

  // BDD サイズのしきい値
  SizeType mSizeLimit{1000};

};

END_NAMESPACE_DRUID

#endif // CALCCNFIMPL2_H
