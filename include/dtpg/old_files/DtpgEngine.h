#ifndef DTPGENGINE_H
#define DTPGENGINE_H

/// @file DtpgEngine.h
/// @brief DtpgEngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "BoolDiffEngine.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "NodeTimeValList.h"
#include "Justifier.h"

#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine DtpgEngine.h "DtpgEngine.h"
/// @brief DTPG の基本的な処理を行うクラス
///
/// BoolDiffEngine の周りに故障検出条件を付加したもの．
///
/// option の仕様は以下の通り
/// - "sat_param": JSONオブジェクト
///                SATソルバの初期化パラメータ
/// - "extractor": JSONオブジェクト(null or string)
///                extractor の初期化パラメータ
/// - "justifier": JSONオブジェクト(null or string)
///                justifier の初期化パラメータ
//////////////////////////////////////////////////////////////////////
class DtpgEngine :
  public BoolDiffEngine
{
public:

  /// @brief コンストラクタ
  DtpgEngine(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* root,	   ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option        ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  virtual
  ~DtpgEngine();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタン生成を行う．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 故障
  );

  /// @brief solve() が成功した時にテストパタンを生成する．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 故障
  );

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  NodeTimeValList
  get_sufficient_condition(
    const TpgFault* fault ///< [in] 故障
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点ノードを返す．
  virtual
  const TpgNode*
  fault_origin(
    const TpgFault* fault ///< [in] 対象の故障
  ) = 0;

  /// @brief 故障の活性化条件
  ///
  /// fault_origin() まで故障の影響が伝搬する条件を返す．
  virtual
  NodeTimeValList
  fault_condition(
    const TpgFault* fault ///< [in] 対象の故障
  ) = 0;

  /// @brief gen_pattern() で用いる追加の検出条件
  ///
  /// デフォルトでは空
  virtual
  vector<SatLiteral>
  extra_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  );

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE_H
