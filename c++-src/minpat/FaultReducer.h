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
#include "TestVector.h"
#include "FaultInfo.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FFRFaultList;

//////////////////////////////////////////////////////////////////////
/// @class FaultReducer FaultReducer.h "FaultReducer.h"
/// @brief 支配故障を求めて対象の故障を削減するクラス
//////////////////////////////////////////////////////////////////////
class FaultReducer
{
public:

  /// @brief コンストラクタ
  FaultReducer(
    const TpgNetwork& network,   ///< [in] 対象のネットワーク
    const JsonValue& option = {} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultReducer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 支配関係を用いて故障を削減する．
  /// @return 削減した故障情報のリストを返す．
  vector<FaultInfo>
  run(
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障のリスト
    const vector<TestVector>& tv_list          ///< [in] テストパタンのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @grep 故障シミュレーションを用いて被支配故障の候補を生成する．
  ///
  /// 結果は mDomCandListArray に格納される．
  void
  gen_dom_cands(
    const vector<const TpgFault*>& fault_list,
    const vector<TestVector>& tv_list
  );

  /// @brief 同一FFR内の支配関係を用いて故障を削減する．
  void
  ffr_reduction(
    const FFRFaultList& ffr_fault_list
  );

  /// @brief 故障の解析を行う．
  ///
  /// 結果は FaultInfo.mSuffCond, mMandCond に格納される．
  void
  fault_analysis(
    const FFRFaultList& ffr_fault_list
  );

  /// @brief trivial な故障間の支配関係のチェックを行う．
  void
  trivial_reduction1(
    const FFRFaultList& ffr_fault_list
  );

  /// @brief trivial な故障に支配されている場合のチェックを行う．
  void
  trivial_reduction2(
    const FFRFaultList& ffr_fault_list
  );

  /// @brief trivial な故障が支配されている場合のチェックを行う．
  void
  trivial_reduction3(
    const FFRFaultList& ffr_fault_list
  );

  /// @brief 異なる FFR 間の支配故障のチェックを行う．
  void
  global_reduction(
    const FFRFaultList& ffr_fault_list,
    bool skip_trivial
  );

  /// @brief fault に支配されている故障の候補リストを返す．
  const vector<const TpgFault*>&
  dom_cand_list(
    const TpgFault* fault
  ) const
  {
    return mDomCandListArray[fault->id()];
  }

  /// @brief fault を支配する故障の候補リストを返す．
  const vector<const TpgFault*>&
  rev_cand_list(
    const TpgFault* fault
  ) const
  {
    return mFaultInfoArray[fault->id()].mRevCandList;
  }

  /// @brief fault が削除されているか調べる．
  bool
  is_deleted(
    const TpgFault* fault
  ) const
  {
    return mFaultInfoArray[fault->id()].mDelMark;
  }

  /// @brief fault に削除された印をつける．
  void
  set_deleted(
    const TpgFault* fault
  )
  {
    mFaultInfoArray[fault->id()].mDelMark = true;
    -- mFaultNum;
  }

  /// @brief trivial fault か調べる．
  bool
  is_trivial(
    const TpgFault* fault
  )
  {
    return mFaultInfoArray[fault->id()].mTrivial;
  }

  /// @brief fault の十分条件を得る．
  const NodeTimeValList&
  sufficient_condition(
    const TpgFault* fault
  ) const
  {
    return mFaultInfoArray[fault->id()].mSuffCond;
  }

  /// @brief fault の必要条件を得る．
  const NodeTimeValList&
  mandatory_condition(
    const TpgFault* fault
  ) const
  {
    return mFaultInfoArray[fault->id()].mMandCond;
  }

  /// @brief 2つの FFR が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFFR* ffr1,
    const TpgFFR* ffr2
  );

  /// @brief 2つの故障が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFault* fault1,
    const TpgFault* fault2
  );

  /// @brief 2つの故障が共通部分を持つか調べる．
  bool
  check_intersect(
    const TpgFault* fault1,
    const TpgFFR* ffr2
  );

  /// @brief check_intersect() の下請け関数
  bool
  _check_intersect(
    SizeType id1,
    SizeType id2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障に関する追加情報
  struct Info {
    // 削除マーク
    bool mDelMark{false};
    // 十分条件と必要条件が等しいとき true
    bool mTrivial{false};
    // 支配故障の候補リスト
    vector<const TpgFault*> mRevCandList;
    // 十分条件
    NodeTimeValList mSuffCond;
    // 必要条件
    NodeTimeValList mMandCond;
  };

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // オプション
  JsonValue mOption;

  // 故障番号をキーとして所属する FFR 番号を格納する配列
  vector<SizeType> mFFRMap;

  // FFR の TFO の TFI に含まれる入力ノード番号のリスト
  vector<vector<SizeType>> mInputListArray;

  // 支配故障の候補リストの配列
  // キーは故障番号
  vector<vector<const TpgFault*>> mDomCandListArray;

  // 故障情報の配列
  // キーは故障番号
  vector<Info> mFaultInfoArray;

  // 残っている故障数
  SizeType mFaultNum;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // FAULTREDUCER_H
