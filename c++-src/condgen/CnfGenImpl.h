#ifndef CNFGENIMPL_H
#define CNFGENIMPL_H

/// @file CnfGenImpl.h
/// @brief CnfGenImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "AssignMap.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenImpl CnfGenImpl.h "CnfGenImpl.h"
/// @brief CnfGen の下請けクラス
//////////////////////////////////////////////////////////////////////
class CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenImpl(
    StructEngine& engine,  ///< [in] StructEngine
    const AssignMap& map   ///< [in] 変数番号の変換マップ
  ) : mEngine{engine},
      mMap{map}
  {
  }

  /// @brief デストラクタ
  ~CnfGenImpl() = default;


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
  static
  CnfSize
  calc_cnf_size(
    const Expr& expr
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の下請け関数
  /// @return expr が成り立つ時に 1 になるリテラルを返す．
  SatLiteral
  make_cnf_sub(
    const Expr& expr ///< [in] 式
  );

  /// @brief XOR を表す CNF を生成する．
  void
  make_xor_cnf(
    SatLiteral lit,                     ///< [in] 出力のリテラル
    const vector<SatLiteral>& opr_lits, ///< [in] オペランドのリテラル
    SizeType begin,                     ///< [in] 開始位置
    SizeType end                        ///< [in] 終了位置
  );

  /// @brief calc_cnf_size() の下請け関数
  static
  CnfSize
  calc_sub(
    const Expr& expr ///< [in] 式
  );

  /// @brief XOR 用のCNFサイズ計算関数
  static
  CnfSize
  calc_xor(
    SizeType n ///< [in] 入力数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // StructEngine
  StructEngine& mEngine;

  // 変数番号の変換マップ
  const AssignMap& mMap;

};

END_NAMESPACE_DRUID

#endif // CNFGENIMPL_H
