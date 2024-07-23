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

class TestCover;
class FaultInfoMgr;

//////////////////////////////////////////////////////////////////////
/// @class TestCoverGen TestCoverGen.h "TestCoverGen.h"
/// @brief 各故障のテストカバーを生成するクラス
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
  vector<TestCover>
  run(
    const FaultInfoMgr& mgr, ///< [in] 故障の情報を持つオブジェクト
    const JsonValue& option  ///< [in] オプション
    = JsonValue{}            ///<      - 'sat_param': object   SATソルバ用のパラメータ
                             ///<      - 'cube_per_fault': int 1故障あたりのキューブ数
                             ///<      - 'debug': bool         デバッグフラグ
  );

};

END_NAMESPACE_DRUID

#endif // TESTCOVERGEN_H
