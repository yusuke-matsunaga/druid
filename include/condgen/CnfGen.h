#ifndef CNFGEN_H
#define CNFGEN_H

/// @file CnfGen.h
/// @brief CnfGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/sat.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class StructEngine;
class AssignExpr;

//////////////////////////////////////////////////////////////////////
/// @class CnfGen CnfGen.h "CnfGen.h"
/// @brief AssignExpr を CNF に変換するクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class CnfGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 論理式を CNF に変換する．
  /// @return 活性化するための条件を返す．
  static
  vector<SatLiteral>
  make_cnf(
    StructEngine& engine,   ///< [in] StructEngine
    const AssignExpr& expr, ///< [in] 式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 複数の論理式を CNF に変換する．
  /// @return 個々の式の活性化するための条件のリストを返す．
  static
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,                ///< [in] StructEngine
    const vector<AssignExpr>& expr_list, ///< [in] 式のリスト
    const JsonValue& option              ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 論理式を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const AssignExpr& expr, ///< [in] 式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const vector<AssignExpr>& expr_list, ///< [in] 式のリスト
    const JsonValue& option              ///< [in] オプション
    = JsonValue{}
  );

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
