#ifndef EXPRGEN_H
#define EXPRGEN_H

/// @file ExprGen.h
/// @brief ExprGen のヘッダファイル
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
    const TpgFFR* ffr,                    ///< [in] 対象のFFR
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
  SizeType
  run(
    const TpgFault* fault
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief expr を否定した節を加える．
  void
  add_negation(
    SatLiteral clit,
    const AssignExpr& expr
  );

  /// @brief expr を否定した節を加える．
  SatLiteral
  add_negation_sub(
    const AssignExpr& expr
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のFFR
  const TpgFFR* mFFR;

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
