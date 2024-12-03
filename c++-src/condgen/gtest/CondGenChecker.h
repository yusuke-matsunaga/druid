#ifndef CONDGENCHECKER_H
#define CONDGENCHECKER_H

/// @file CondGenChecker.h
/// @brief CondGenChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "AssignExpr.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CondGenChecker CondGenChecker.h "CondGenChecker.h"
/// @brief CondGen の結果の検証を行うクラス
//////////////////////////////////////////////////////////////////////
class CondGenChecker
{
public:

  /// @brief コンストラクタ
  CondGenChecker(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const AssignExpr& cond,               ///< [in] CondGen::root_cond() の結果
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief コンストラクタ
  CondGenChecker(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const TpgFault* fault,                ///< [in] 対象の故障
    const AssignExpr& cond,               ///< [in] CondGen::root_cond() の結果
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~CondGenChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif 結果の検証を行う．
  bool
  check();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief cond の条件を CNF式に変換する．
  void
  make_cnf(
    const Expr& expr,
    const AssignMap& assign_map
  );

  /// @brief cond の条件を CNF式に変換する．
  SatLiteral
  make_cnf2(
    const Expr& expr,
    const AssignMap& assign_map
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本エンコーダ
  StructEngine mEngine;

  // FFR用のブール微分器
  BoolDiffEnc* mBdEnc;

  // 故障検出の追加条件
  AssignList mExtraCond;

  // 対象の条件
  AssignExpr mCond;

};

END_NAMESPACE_DRUID

#endif // CONDGENCHECKER_H
