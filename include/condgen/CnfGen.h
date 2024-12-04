#ifndef CNFGEN_H
#define CNFGEN_H

/// @file CnfGen.h
/// @brief CnfGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"
#include "ym/sat.h"


BEGIN_NAMESPACE_DRUID

class StructEngine;
class AssignEnxpr;

//////////////////////////////////////////////////////////////////////
/// @class CnfSize CnfSize.h "CnfSize.h"
/// @brief CNF のサイズを表す構造体
//////////////////////////////////////////////////////////////////////
struct CnfSize
{
  SizeType clause_num{0};  ///< 項数
  SizeType literal_num{0}; ///< リテラル数

  CnfSize&
  operator+=(
    const CnfSize& right
  )
  {
    clause_num += right.clause_num;
    literal_num += right.literal_num;
    return *this;
  }

  CnfSize
  operator+(
    const CnfSize& right
  ) const
  {
    return CnfSize{*this}.operator+=(right);
  }

};


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

  /// @brief AssignExpr を CNF に変換する．
  /// @return expr の条件を活性化する時の assumption を返す．
  static
  vector<SatLiteral>
  make_cnf(
    StructEngine& engine,   ///< [in] StructEngine
    const AssignExpr& expr, ///< [in] 元の式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief AssignExpr を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const AssignExpr& expr, ///< [in] 元の式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
