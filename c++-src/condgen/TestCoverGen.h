#ifndef TESTCOVERGEN_H
#define TESTCOVERGEN_H

/// @file TestCoverGen.h
/// @brief TestCoverGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultInfoMgr;
class TestCond;

//////////////////////////////////////////////////////////////////////
/// @class TestCoverGen TestCoverGen.h "TestCoverGen.h"
/// @brief 各故障の検出条件を生成するクラス
///
/// 実はクラスメソッドだけの仮想クラス
//////////////////////////////////////////////////////////////////////
class TestCoverGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各故障のテストカバーを生成する．
  static
  vector<TestCond>
  run(
    const TpgNetwork& network,                 ///< [in] ネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 故障のリスト
    SizeType limit,                            ///< [in] ループ回数の上限
    const JsonValue& option  ///< [in] オプション
    = JsonValue{}            ///<      - 'sat_param': object   SATソルバ用のパラメータ
                             ///<      - 'debug': bool         デバッグフラグ
  );

};

END_NAMESPACE_DRUID

#endif // TESTCOVERGEN_H
