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
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

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
  /// @return 削減した故障のリストを返す．
  vector<const TpgFault*>
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
  /// @return 削減した故障リストを返す．
  vector<const TpgFault*>
  ffr_reduction(
    const vector<const TpgFault*>& fault_list
  );

  /// @brief 故障の解析を行う．
  ///
  /// 結果は mSuffCondArray, mMandCondArray に格納される．
  void
  fault_analysis(
    const vector<const TpgFault*>& fault_list
  );

  /// @brief 異なる FFR 間の支配故障のチェックを行う．
  vector<const TpgFault*>
  global_reduction(
    const vector<const TpgFault*>& fault_list
  );

  /// @brief fault に支配されている故障の候補リストを返す．
  const vector<const TpgFault*>&
  dom_cand_list(
    const TpgFault* fault
  ) const
  {
    return mDomCandListArray[fault->id()];
  }

  /// @brief fault が削除されているか調べる．
  bool
  is_deleted(
    const TpgFault* fault
  ) const
  {
    return mDelMark[fault->id()];
  }

  /// @brief fault に削除された印をつける．
  void
  set_deleted(
    const TpgFault* fault
  )
  {
    mDelMark[fault->id()] = true;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // オプション
  JsonValue mOption;

  // 支配故障の候補リストの配列
  // キーは故障番号
  vector<vector<const TpgFault*>> mDomCandListArray;

  // 削除マークの配列
  // キーは故障番号
  vector<bool> mDelMark;

  // 故障の十分割り当ての配列
  // キーは故障番号
  vector<NodeTimeValList> mSuffCondArray;

  // 故障の必須割り当ての配列
  // キーは故障番号
  vector<NodeTimeValList> mMandCondArray;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // FAULTREDUCER_H
