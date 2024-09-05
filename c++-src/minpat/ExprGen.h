#ifndef EXPRGEN_H
#define EXPRGEN_H

/// @file ExprGen.h
/// @brief ExprGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"
#include "BoolDiffEnc.h"
#include "TestCover.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExprGen ExprGen.h "ExprGen.h"
/// @brief 拡張テストキューブを生成するクラス
///
/// 拡張テストキューブを生成する．
/// 拡張テストキューブとは内部の信号線を含んだブール空間上で
/// 故障検出できるキューブのこと
///
/// パラメータ
/// - "debug":          bool   デバッグフラグ
/// - "cube_per_fault"  int    1故障あたりのキューブ数の上限
/// - "dtpg":           object DTPG用の初期化パラメータ
//////////////////////////////////////////////////////////////////////
class ExprGen
{
public:

  /// @brief コンストラクタ
  ExprGen(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgNode* root                   ///< [in] FFRの根のノード
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );


  /// @brief デストラクタ
  ~ExprGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のFFRの根のノードのブール微分を表す論理式を得る．
  /// @return 生成した式を返す．
  AssignExpr
  run();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFRの根のノード
  const TpgNode* mRoot;

  // 基本エンコーダ
  StructEngine mEngine;

  // FFR用のブール微分器
  BoolDiffEnc* mBdEnc;

  // FFR の出力の故障伝搬の必要条件
  AssignList mRootMandCond;

  // FFR の出力の故障伝搬可能性
  SatBool3 mRootStatus;

  // 上限値
  SizeType mLimit;

  // デバッグフラグ
  int mDebug{0};

};

END_NAMESPACE_DRUID

#endif // EXPRGEN_H
