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
#include "DetCond.h"
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
  check(
    const AssignList& extra_cond, ///< [in] 追加の条件
    const DetCond& cond           ///< [in] CondGen::root_cond() の結果
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 基本エンコーダ
  std::unique_ptr<StructEngine> mEngine;

  // FFR用のブール微分器
  BoolDiffEnc* mBdEnc;

  // 故障検出の追加条件
  AssignList mExtraCond;

  // 対象の条件
  AssignExpr mCond;

};

END_NAMESPACE_DRUID

#endif // CONDGENCHECKER_H
