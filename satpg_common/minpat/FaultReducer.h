#ifndef FAULTREDUCER_H
#define FAULTREDUCER_H

/// @file FaultReducer.h
/// @brief FaultReducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/McMatrix.h"
#include "ym/SatSolverType.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class FaultReducer FaultReducer.h "FaultReducer.h"
/// @brief 支配故障を求めるクラスn
//////////////////////////////////////////////////////////////////////
class FaultReducer
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  FaultReducer(const TpgNetwork& network,
	       FaultType fault_type);

  /// @brief デストラクタ
  ~FaultReducer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif 内部で用いる SAT ソルバのタイプの設定を行う．
  /// @param[in] solver_type SATソルバのタイプ
  void
  set_solver_type(const SatSolverType& solver_type);

  /// @brief デバッグフラグをセットする．
  /// @param[in] debug 設定する値 (true/false)
  void
  set_debug(bool debug);

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  /// @param[inout] fault_list 対象の故障リスト
  /// @param[in] algorithm アルゴリズム
  void
  fault_reduction(vector<const TpgFault*>& fault_list,
		  const string& algorithm);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部のデータ構造を初期化する．
  /// @param[in] fi_list 故障情報のリスト
  void
  init(const vector<const TpgFault*>& fault_list,
       bool need_mand_cond);

  /// @brief 故障シミュレーションを行って支配故障の候補を作る．
  /// @param[in] loop_limit 変化がなくなってから繰り返すループ数
  void
  make_dom_candidate(int loop_limit);

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
  dom_reduction1();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction2();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction3();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction4();

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
  StopWatch mTimer;

};

END_NAMESPACE_YM_SATPG

#endif // FAULTREDUCER_H
