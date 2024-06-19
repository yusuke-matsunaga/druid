#ifndef FAULTREDUCER_H
#define FAULTREDUCER_H

/// @file FaultReducer.h
/// @brief FaultReducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFault.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/SatInitParam.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//class FaultInfo;

//////////////////////////////////////////////////////////////////////
/// @class FaultReducer FaultReducer.h "FaultReducer.h"
/// @brief 支配故障を求めるクラス
//////////////////////////////////////////////////////////////////////
class FaultReducer
{
public:

  /// @brief コンストラクタ
  FaultReducer(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障リスト
    const vector<TestVector>& tv_list,         ///< [in] テストベクタのリスト
    const JsonValue& option = {}               ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultReducer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  /// @return 代表故障のリストを返す．
  vector<const TpgFault*>
  run();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部のデータ構造を初期化する．
  void
  init(
    bool need_mand_cond
  );

  /// @brief 同一 FFR 内の支配故障のチェックを行う．
  void
  ffr_reduction();

#if 0
  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction1();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction2();

  /// @brief 異なる FFR 間の支配故障のチェックを行う．
  void
  dom_reduction3();
#else
  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction();
#endif

  /// @brief 対象の故障が削除されていたら true を返す．
  bool
  is_deleted(
    const TpgFault* fault ///< [in] 対象の故障
  ) const
  {
    return mDeleted[fault->id()];
  }

  /// @brief 支配された故障を削除する．
  void
  delete_fault(
    const TpgFault* fault ///< [in] 対象の故障
  )
  {
    // 削除済みマークをつける．
    mDeleted[fault->id()] = true;
    // 不要となったベクタの領域を解放するハックコード
    vector<const TpgFault*>{}.swap(mDomCandList[fault->id()]);
  }

  /// @brief mFaultList 中の mDeleted マークが付いていない故障数を数える．
  SizeType
  count_faults() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

#if 0
  // 故障情報のリスト
  const vector<FaultInfo>& mFaultInfoList;
#else
  // 故障のリスト
  const vector<const TpgFault*>& mFaultList;

  // テストベクタのリスト
  const vector<TestVector>& mTvList;
#endif

  // FFR ごとの故障リストの配列
  vector<vector<const TpgFault*>> mFFRFaultList;

  // 故障番号をキーにして削除されたかを表すフラグの配列
  vector<bool> mDeleted;

  // 支配故障の候補のリストの配列
  vector<vector<const TpgFault*>> mDomCandList;

  // 故障シミュレータの制御パラメータ
  SizeType mLoopLimit{1};

  // FFRChecker 用のSATパラメータ
  JsonValue mFFRCheckerOption;

  // DomChecker 用のSATパラメータ
  SatInitParam mDomCheckerParam;

  // UndetChecker 用のSATパラメータ
  SatInitParam mUndetCheckerParam;

#if 0
  // アルゴリズム
  string mAlgorithm;

  // simple オプション
  bool mSimple;
#endif

  // デバッグフラグ
  bool mDebug{false};

  // 計時を行うオブジェクト
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // FAULTREDUCER_H
