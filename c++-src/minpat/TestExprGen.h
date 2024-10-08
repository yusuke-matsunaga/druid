#ifndef TESTEXPRGEN_H
#define TESTEXPRGEN_H

/// @file TestExprGen.h
/// @brief TestExprGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultInfoMgr;
class TestCover;

//////////////////////////////////////////////////////////////////////
/// @class TestExprGen TestExprGen.h "TestExprGen.h"
/// @brief 各故障のテストカバーを生成するクラス
///
/// 実はクラスメソッドだけの仮想クラス
//////////////////////////////////////////////////////////////////////
class TestExprGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各故障のテストカバーを生成する．
  static
  SizeType
  run(
    const TpgNetwork& network,                 ///< [in] ネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 故障のリスト
    const JsonValue& option  ///< [in] オプション
    = JsonValue{}            ///<      - 'sat_param': object   SATソルバ用のパラメータ
                             ///<      - 'cube_per_fault': int 1故障あたりのキューブ数
                             ///<      - 'debug': bool         デバッグフラグ
  );

  /// @brief 各故障のテストカバーを生成する．
  static
  vector<TestCover>
  run(
    const FaultInfoMgr& finfo_mgr, ///< [in] 故障情報を持つオブジェクト
    const JsonValue& option  ///< [in] オプション
    = JsonValue{}            ///<      - 'sat_param': object   SATソルバ用のパラメータ
                             ///<      - 'cube_per_fault': int 1故障あたりのキューブ数
                             ///<      - 'debug': bool         デバッグフラグ
  );

};

END_NAMESPACE_DRUID

#endif // TESTEXPRGEN_H
