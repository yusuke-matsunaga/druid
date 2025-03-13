#ifndef CONDGENMGR_H
#define CONDGENMGR_H

/// @file CondGenMgr.h
/// @brief CondGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenMgr CondGenMgr.h "CondGenMgr.h"
/// @brief FFRの故障伝搬条件を表すCNFを作るクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class CondGenMgr
{
public:

  /// @brief 検出条件を表す構造体
  struct CondLits {
    SizeType id;                  ///< FFR 番号
    bool detected;                ///< 検出可の時 true となるフラグ
    std::vector<SatLiteral> lits; ///< 検出条件を表すリテラルのリスト
  };


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
  std::vector<CondLits>
  make_cnf(
    StructEngine& engine,                  ///< [in] CNFの作成用のエンジン
    const std::vector<DetCond>& cond_list, ///< [in] 条件のリスト
    const JsonValue& option                ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
