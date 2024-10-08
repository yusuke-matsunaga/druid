#ifndef EXCUBEGEN_H
#define EXCUBEGEN_H

/// @file ExCubeGen.h
/// @brief ExCubeGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"
#include "TestCover.h"
#include "ym/Bdd.h"
#include "ym/BddMgr.h"
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

  /// @brief コンストラクタ
  ExCubeGen(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFFR* ffr,                    ///< [in] 対象の FFR
    const AssignList& root_cond,          ///< [in] 出力の故障伝搬の必要条件
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

  /// @brief TestCover に対応するBDDを返す．
  Bdd
  make_bdd(
    const TestCover& cover
  );

  /// @brief AssignList に対応するBDDを返す．
  Bdd
  make_bdd(
    const AssignList& cube
  );

  /// @brief BddMgr を返す．
  BddMgr&
  bddmgr()
  {
    return mBddMgr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Assign に対応するBDDを返す．
  Bdd
  make_bdd(
    const Assign& assign
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

  // 上限値
  SizeType mLimit;

  // BDDマネージャ
  BddMgr mBddMgr;

  // TpgNode とBDDの変数の対応付けを持つ辞書
  // 実際には時刻も区別する．
  std::unordered_map<SizeType, Bdd> mBddMap;

  // 次のBDDの変数の値
  SizeType mBddVarBase;

  // デバッグフラグ
  int mDebug{0};

};

END_NAMESPACE_DRUID

#endif // EXCUBEGEN_H
