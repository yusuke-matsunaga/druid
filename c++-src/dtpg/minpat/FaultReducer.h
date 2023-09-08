#ifndef FAULTREDUCER_H
#define FAULTREDUCER_H

/// @file FaultReducer.h
/// @brief FaultReducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/McMatrix.h"
#include "ym/SatSolverType.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultReducer FaultReducer.h "FaultReducer.h"
/// @brief 支配故障を求めるクラス
//////////////////////////////////////////////////////////////////////
class FaultReducer
{
public:

  /// @brief コンストラクタ
  FaultReducer(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    FaultType fault_type       ///< [in] 故障の種類
  );

  /// @brief デストラクタ
  ~FaultReducer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif 内部で用いる SAT ソルバのタイプの設定を行う．
  void
  set_solver_type(
    const SatSolverType& solver_type ///< [in] SATソルバのタイプ
  );

  /// @brief デバッグフラグをセットする．
  void
  set_debug(
    bool debug ///< [in] 設定する値 (true/false)
  );

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  void
  fault_reduction(
    vector<const TpgFault*>& fault_list, ///< [inout] 対象の故障リスト
    const string& algorithm ///< [in] アルゴリズム
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部のデータ構造を初期化する．
  void
  init(
    const vector<const TpgFault*>& fault_list, ///< [in] 故障情報のリスト
    bool need_mand_cond
  );

  /// @brief 故障シミュレーションを行って支配故障の候補を作る．
  void
  make_dom_candidate(
    int loop_limit ///< [in] 変化がなくなってから繰り返すループ数
  );

  /// @brief 一回の故障シミュレーションを行う．
  /// @retval true 支配故障のリストに変化があった．
  /// @retval false 変化がなかった．
  bool
  do_fsim();

  /// @brief 同一 FFR 内の支配故障のチェックを行う．
  void
  ffr_reduction();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction1(
    bool simple
  );

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction2();

  ///< [inout] fi_list 故障情報のリスト
  void
  dom_reduction3(
    bool simple
  );

  /// @brief mFaultList 中の mDeleted マークが付いていない故障数を数える．
  int
  count_faults() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障に関するいくつかの情報をまとめたもの
  struct FaultInfo
  {
    // 削除マーク
    bool mDeleted;

    // 故障シミュレーションの検出パタン
    PackedVal mPat;

    // FFR 内の伝搬条件
    NodeValList mFFRCond;

    // 十分条件
    NodeValList mSuffCond;

    // 必要条件
    NodeValList mMandCond;

    // この故障が支配している故障の候補リスト
    vector<const TpgFault*> mDomCandList;

    // 検出回数
    int mDetCount;

  };

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 故障シミュレータ
  Fsim mFsim;

  // SATソルバのタイプ
  SatSolverType mSolverType;

  // デバッグフラグ
  bool mDebug;

  // 故障リスト
  vector<const TpgFault*> mFaultList;

  // 故障に関する情報を入れた配列
  vector<FaultInfo> mFaultInfoArray;

  // テストベクタのリスト
  vector<TestVector> mTvList;

  // 計時を行うオブジェクト
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // FAULTREDUCER_H
