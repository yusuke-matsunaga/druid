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

  /// @brief CNFを生成する．
  static
  vector<SatLiteral>
  make_cnf(
    StructEngine& engine,   ///< [in] 基本エンジン
    const DetCond& cond,    ///< [in] 条件
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  )
  {
    auto tmp_list = make_cnf(engine, vector<DetCond>{cond}, option);
    ASSERT_COND( tmp_list.size() == 1 );
    return tmp_list.front();
  }

  /// @brief CNFを生成する．
  static
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,             ///< [in] 基本エンジン
    const vector<DetCond>& cond_list, ///< [in] 式のリスト
    const JsonValue& option           ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 複数の論理式を CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_cnf_size(
    const TpgNetwork& network,        ///< [in] 対象のネットワーク
    const vector<DetCond>& cond_list, ///< [in] 式のリスト
    const JsonValue& option           ///< [in] オプション
    = JsonValue{}
  );

  /// @brief 複数の論理式を CNF に変換する．
  /// @return 個々の式の活性化するための条件のリストを返す．
  static
  vector<vector<SatLiteral>>
  make_raw_cnf(
    StructEngine& engine,     ///< [in] StructEngine
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief 複数の論理式をそのまま CNF に変換した際の項数とリテラル数を数える．
  static
  CnfSize
  calc_raw_cnf_size(
    const TpgNetwork& network ///< [in] 対象のネットワーク
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
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    const vector<DetCond>& expr_list ///< [in] 式のリスト
  );

};

END_NAMESPACE_DRUID

#endif // CNFGENMGR_H
