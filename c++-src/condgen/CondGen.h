#ifndef CONDGEN_H
#define CONDGEN_H

/// @file CondGen.h
/// @brief CondGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "condgen/DetCond.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGen CondGen.h "CondGen.h"
/// @brief 拡張テストキューブを生成するクラス
///
/// 拡張テストキューブを生成する．
/// 拡張テストキューブとは内部の信号線を含んだブール空間上で
/// 故障検出できるキューブのこと
///
/// パラメータ
/// - "debug":          int    デバッグレベル
/// - "dtpg":           object DTPG用の初期化パラメータ
//////////////////////////////////////////////////////////////////////
class CondGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRの出力の故障伝搬条件を求める．
  /// @return 条件を返す．
  static
  DetCond
  root_cond(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR& ffr,                    ///< [in] 対象の FFR
    SizeType limit,                       ///< [in] ループ回数の上限
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // CONDGEN_H
