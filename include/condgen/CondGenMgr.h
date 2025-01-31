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
#include "ym/CnfSize.h"
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
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRの故障伝搬条件を求める．
  /// @return 各 FFR の伝搬条件を表すリテラル集合のリストを返す．
  static
  vector<vector<SatLiteral>>
  make_ffr_cond(
    StructEngine& engine,                ///< [in] CNF作成用のエンジン
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    const JsonValue& option              ///< [in] オプション
  );

  /// @brief FFRの故障伝搬条件を表すCNFのサイズを求める．
  static
  CnfSize
  calc_ffr_cond_size(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    const JsonValue& option              ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
