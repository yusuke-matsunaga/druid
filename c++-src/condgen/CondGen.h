#ifndef CONDGEN_H
#define CONDGEN_H

/// @file CondGen.h
/// @brief CondGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "TestCond.h"
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
/// - "cube_per_fault"  int    1故障あたりのキューブ数の上限
/// - "dtpg":           object DTPG用の初期化パラメータ
//////////////////////////////////////////////////////////////////////
class CondGen
{
public:

  /// @brief コンストラクタ
  CondGen(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief コンストラクタ
  CondGen(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const AssignList& root_cond,          ///< [in] 出力の故障伝搬の必要条件
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~CondGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFRの出力の故障伝搬条件を求める．
  /// @return 条件式を返す．
  TestCond
  root_cond(
    SizeType limit,      ///< [in] ループ回数の上限
    SizeType& loop_count ///< [out] 実際のループ回数
  );

  /// @brief 与えられた故障を検出する条件を生成する．
  /// @return 条件式を返す．
  TestCond
  fault_cond(
    const TpgFault* fault, ///< [in] 対象の故障
    SizeType limit,        ///< [in] ループ回数の上限
    SizeType& loop_count   ///< [out] 実際のループ回数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief root_cond(), fault_cond() の共通な下請け関数
  TestCond
  gen_cond(
    const AssignList& extra_cond, ///< [in] 追加の条件
    SizeType limit,               ///< [in] ループの上限
    SizeType& loop_count          ///< [out] 実際のループ回数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR
  const TpgFFR* mFFR;

  // 基本エンコーダ
  StructEngine mEngine;

  // FFR用のブール微分器
  BoolDiffEnc* mBdEnc;

  // FFR の出力の故障伝搬の必要条件
  AssignList mRootMandCond;

  // FFR の出力の故障伝搬可能性
  SatBool3 mRootStatus;

  // デバッグフラグ
  int mDebug{0};

};

END_NAMESPACE_DRUID

#endif // CONDGEN_H
