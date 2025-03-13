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
#include "ym/CnfSize.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

/// @brief calc_ffr_cond_size の結果を表す構造体
struct CondGenStats
{
  CnfSize total_raw_size; ///< オリジナルのCNF式のサイズ
  CnfSize naive_size;     ///< SOP をナイーブに CNF に変換した場合のサイズ
  CnfSize opt_size;       ///< 最適化した CNF に変換した場合のサイズ
  SizeType sop_num;       ///< SOPした条件の数
  CnfSize rest_size;      ///< SOP 化できなかった部分のサイズ
  SizeType rest_num;      ///< SOP 化できなかった条件の数
};


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
  vector<DetCond>
  make_cond(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    const JsonValue& option              ///< [in] オプション
  );

  /// @brief FFRの故障伝搬条件を表すCNF式を作る．
  static
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,                ///< [in] CNFの作成用のエンジン
    const vector<DetCond>& cond_list,    ///< [in] 条件のリスト
    const JsonValue& option              ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
