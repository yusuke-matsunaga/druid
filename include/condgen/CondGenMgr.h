#ifndef CONDGENMGR_H
#define CONDGENMGR_H

/// @file CondGenMgr.h
/// @brief CondGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "condgen/DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class CondGenStats;

//////////////////////////////////////////////////////////////////////
/// @class CondGenMgr CondGenMgr.h "CondGenMgr.h"
/// @brief FFRの故障伝搬条件を表すCNFを作るクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class CondGenMgr
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRの故障伝搬条件を求める．
  /// @return 各 FFR の伝搬条件のリストを返す．
  static
  std::vector<DetCond>
  make_cond(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief FFRの故障伝搬条件を表すCNF式を作る．
  static
  std::vector<std::vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,                  ///< [in] CNFの作成用のエンジン
    const std::vector<DetCond>& cond_list, ///< [in] 条件のリスト
    const JsonValue& option,               ///< [in] オプション
    CondGenStats& stats                    ///< [out] 統計情報を入れるオブジェクト
  );

  /// @brief FFRの故障伝搬条件を表すCNF式を作る(ナイーブバージョン)
  static
  std::vector<std::vector<SatLiteral>>
  make_cnf_naive(
    StructEngine& engine,                  ///< [in] CNFの作成用のエンジン
    const JsonValue& option,               ///< [in] オプション
    CondGenStats& stats                    ///< [out] 統計情報を入れるオブジェクト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // make_cnf の内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正常回路用の CNF を作る．
  static
  void
  make_base_cnf(
    StructEngine& engine ///< [in] CNFの作成用のエンジン
  );

  /// @brief DetCond から Expr を作る．
  ///
  /// DetCond.type() が Detected/PartialDetected の場合のみ Expr を作る．
  static
  std::vector<Expr>
  make_expr(
    const std::vector<DetCond>& cond_list, ///< [in] 条件のリスト
    const JsonValue& option                ///< [in] オプション
  );

  /// @brief Expr を CNF に変換する．
  static
  std::vector<std::vector<SatLiteral>>
  expr_to_cnf(
    StructEngine& engine,               ///< [in] CNFの作成用のエンジン
    const std::vector<Expr>& expr_list, ///< [in] 式のリスト
    const JsonValue& option             ///< [in] オプション
  );

  /// @brief BoolDiffEnc を用いた条件を作る．
  ///
  /// DetCond.type() が PartialDetected/Overflow の場合にのみ
  /// BoolDiffEnc を作る．
  static
  std::vector<SatLiteral>
  make_bd(
    StructEngine& engine,                 ///< [in] CNFの作成用のエンジン
    const std::vector<DetCond>& cond_list ///< [in] 条件
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
