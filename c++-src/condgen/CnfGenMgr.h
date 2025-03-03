#ifndef CNFGENMGR_H
#define CNFGENMGR_H

/// @file CnfGenMgr.h
/// @brief CnfGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DetCond.h"
#include "ym/sat.h"
#include "ym/CnfSize.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class StructEngine;

//////////////////////////////////////////////////////////////////////
/// @class CnfGenMgr CnfGenMgr.h "CnfGenMgr.h"
/// @brief DetCond を CNF に変換するクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class CnfGenMgr
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
    const DetCond& expr,    ///< [in] 式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  )
  {
    auto tmp_list = make_cnf(engine, vector<DetCond>{expr}, option);
    ASSERT_COND( tmp_list.size() == 1 );
    return tmp_list.front();
  }

  /// @brief 複数の論理式を CNF に変換する．
  /// @return 個々の式の活性化するための条件のリストを返す．
  static
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,             ///< [in] StructEngine
    const vector<DetCond>& expr_list, ///< [in] 式のリスト
    const JsonValue& option           ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 論理式を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const DetCond& expr,    ///< [in] 式
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  )
  {
    return calc_cnf_size(vector<DetCond>{expr}, option);
  }

  /// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const vector<DetCond>& expr_list, ///< [in] 式のリスト
    const JsonValue& option           ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 複数の論理式を CNF に変換する．
  /// @return 個々の式の活性化するための条件のリストを返す．
  static
  vector<vector<SatLiteral>>
  make_naive_cnf(
    StructEngine& engine,            ///< [in] StructEngine
    const vector<DetCond>& expr_list ///< [in] 式のリスト
  );

  /// @brief 複数の論理式をそのまま CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_naive_cnf_size(
    const vector<DetCond>& expr_list ///< [in] 式のリスト
  );

};

END_NAMESPACE_DRUID

#endif // CNFGENMGR_H
