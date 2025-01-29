#ifndef CONDGENMGR_H
#define CONDGENMGR_H

/// @file CondGenMgr.h
/// @brief CondGenMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DetCond.h"
#include "ym/CnfSize.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenMgr CondGenMgr.h "CondGenMgr.h"
/// @brief 故障の検出条件を作るクラス
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
  root_cond(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    SizeType limit,                      ///< [in] ループ回数の上限
    const JsonValue& option              ///< [in] オプション
  );

  /// @brief FFRの故障伝搬条件を表すCNFのサイズを求める．
  static
  CnfSize
  calc_root_cond_size(
    const TpgNetwork& network,           ///< [in] 対象のネットワーク
    SizeType limit,                      ///< [in] ループ回数の上限
    const JsonValue& option,             ///< [in] オプション
    const JsonValue& option2             ///< [in] CnfGen 用のオプション
  );

  /// @brief 故障検出条件を求める．
  /// @return fault_list に対する検出条件の配列を返す．
  ///
  /// 結果のベクタは故障番号をキーにして対応する検出条件を格納する．
  static
  vector<DetCond>
  fault_cond(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障のリスト
    SizeType limit,                            ///< [in] ループ回数の上限
    const JsonValue& option                    ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGENMGR_H
