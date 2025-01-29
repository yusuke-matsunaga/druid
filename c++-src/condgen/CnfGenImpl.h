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
#include "DetCond.h"
#include "AssignList.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenImpl CnfGenImpl.h "CnfGenImpl.h"
/// @brief CnfGen の下請けクラスの基底クラス
//////////////////////////////////////////////////////////////////////
class CnfGenImpl
{
public:

  /// @brief デストラクタ
  virtual
  ~CnfGenImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を CNF に変換する．
  virtual
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,            ///< [in] StructEngine
    const vector<DetCond>& cond_list ///< [in] 検出条件のリスト
  ) = 0;

  /// @brief 複数の条件を CNF に変換した際の項数とリテラル数を計算する．
  virtual
  CnfSize
  calc_cnf_size(
    const vector<DetCond>& cond_list ///< [in] 検出条件のリスト
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューブを表すリテラルを返す．
  static
  SatLiteral
  cube_to_cnf(
    StructEngine& engine,  ///< [in] StructEngine
    const AssignList& cube ///< [in] キューブ
  )
  {
    auto cube_lit = engine.solver().new_variable(false);
    for ( auto as: cube ) {
      auto lit = engine.conv_to_literal(as);
      engine.solver().add_clause(~cube_lit, lit);
    }
    return cube_lit;
  }

};

END_NAMESPACE_DRUID

#endif // CNFGENIMPL_H
