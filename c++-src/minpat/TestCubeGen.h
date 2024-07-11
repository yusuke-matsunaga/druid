#ifndef TESTCUBEGEN_H
#define TESTCUBEGEN_H

/// @file TestCubeGen.h
/// @brief TestCubeGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestCubeGen TestCubeGen.h "TestCubeGen.h"
/// @brief テストキューブを生成するクラス
///
/// できるだけ多くの故障を検出するテストキューブを生成する．
/// 具体的には故障を検出するテストキューブを一つもしくは複数生成する．
/// 以降はこのテストキューブの集合を故障検出関数の代用と見なす．
///
/// パラメータ
/// - "cube_per_fault": int    故障あたりのテストキューブ生成数
/// - "dtpg":           object DTPG用の初期化パラメータ
//////////////////////////////////////////////////////////////////////
class TestCubeGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられた故障を検出するテストキューブを生成する．
  ///
  /// 結果は fault_list に格納される．
  static
  void
  run(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    vector<FaultInfo>& fault_list,        ///< [inout] 対象の故障情報のリスト
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // TESTCUBEGEN_H
