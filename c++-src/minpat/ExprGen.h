#ifndef EXCUBEGEN_H
#define EXCUBEGEN_H

/// @file ExCubeGen.h
/// @brief ExCubeGen のヘッダファイル
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
/// @class ExCubeGen ExCubeGen.h "ExCubeGen.h"
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
class ExCubeGen
{
public:

  /// @brief コンストラクタ
  ExCubeGen(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );


  /// @brief デストラクタ
  ~ExCubeGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられた故障を検出するテストカバーを生成する．
  /// @return 生成したテストカバーを返す．
  TestCover
  run(
    const TpgFault* fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR
  const TpgFFR* mFFR;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // FFR用のブール微分器
  BoolDiffEnc* mBdEnc;

  // 上限値
  SizeType mLimit;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // EXCUBEGEN_H
