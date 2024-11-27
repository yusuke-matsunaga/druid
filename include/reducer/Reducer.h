#ifndef REDUCER_H
#define REDUCER_H

/// @file Reducer.h
/// @brief Reducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfoMgr.h"
#include "DomCandMgr.h"
#include "FFRFaultList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

/// @brief 同一FFR内の支配故障のチェックを行う．
vector<const TpgFault*>
ffr_reduction(
  const TpgNetwork& network,                     ///< [in] 対象のネットワーク
  const vector<const TpgFault*>& src_fault_list, ///< [in] 元の故障リスト
  const DomCandMgr& mgr,                         ///< [in] 支配故障の候補リスト
  const JsonValue& option                        ///< [in] オプション
  = JsonValue{}
);

/// @brief 異なる FFR 間の支配故障のチェックを行う．
vector<const TpgFault*>
global_reduction(
  const TpgNetwork& network,                     ///< [in] 対象のネットワーク
  const vector<const TpgFault*>& src_fault_list, ///< [in] 元の故障リスト
  const DomCandMgr& mgr,			 ///< [in] 支配故障の候補リスト
  const JsonValue& option                        ///< [in] オプション
);


//////////////////////////////////////////////////////////////////////
/// @class Reducer Reducer.h "Reducer.h"
/// @brief 支配関係を用いて故障を削減するクラス
///
/// 実はクラスメソッドを呼べばインスタンスには触る必要はない．
//////////////////////////////////////////////////////////////////////
class Reducer
{
public:

  /// @brief 支配関係を用いて故障を削減する．
  /// @return 削減した故障リストを返す．
  static
  vector<const TpgFault*>
  reduce(
    FaultInfoMgr& finfo_mgr, ///< [in] 故障情報を管理するクラス
    const JsonValue& option  ///< [in] オプション
    = JsonValue{}            ///<      - 'sat_param': object SATソルバ用のパラメータ
                             ///<      - 'loop_limit': int   シミュレーション回数
                             ///<      - 'no_analyze': bool  制御フラグ
                             ///<      - 'debug': int        デバッグフラグ
  );


private:

  /// @brief コンストラクタ
  Reducer(
    FaultInfoMgr& finfo_mgr ///< [in] 故障情報を管理するクラス
  );

  /// @brief デストラクタ
  ~Reducer() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障削減する．
  ///
  /// 結果は FaultInfoMgr に格納する．
  void
  run(
    const JsonValue& option ///< [in] オプション
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
    SizeType limit, ///< [in] シミュレーション回数を制御するパラメータ
    int debug      ///< [in] デバッグフラグ
  );

  /// @brief 同一FFR内の支配関係を用いて故障を削減する．
  void
  ffr_reduction(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief trivial な故障間の支配関係のチェックを行う．
  void
  trivial_reduction1(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief trivial な故障に支配されている場合のチェックを行う．
  void
  trivial_reduction2(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief trivial な故障が支配されている場合のチェックを行う．
  void
  trivial_reduction3(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief 異なる FFR 間の支配故障のチェックを行う．
  void
  global_reduction(
    const JsonValue& option, ///< [in] オプション
    bool skip_trivial        ///< [in] trivial な故障をスキップする時 true
  );

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mMgr.network();
  }

  /// @brief 故障を持つFFRのリストを返す．
  const vector<const TpgFFR*>&
  ffr_list() const
  {
    return mFFRFaultList.ffr_list();
  }

  /// @brief FFRごとの故障リストを返す．
  const vector<const TpgFault*>&
  fault_list(
    const TpgFFR* ffr ///< [in] 対象のFFR
  ) const
  {
    return mFFRFaultList.fault_list(ffr);
  }

  /// @brief 故障情報を返す．
  const FaultInfo&
  fault_info(
    const TpgFault* fault ///< [in] 故障
  ) const
  {
    return mMgr.fault_info(fault);
  }

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
    return mRevCandListArray[fault->id()];
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


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct Key {
    SizeType fault_id;
    SizeType ffr_id;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  FaultInfoMgr& mMgr;

  // 現在残っている故障数
  SizeType mFaultNum;

  // FFR ごとの故障リスト
  FFRFaultList mFFRFaultList;

  // 各FFRに関係する入力ノードのリストを収める配列
  // キーは FFR 番号
  vector<vector<SizeType>> mInputListArray;

  // 支配故障の候補リストの配列
  // キーは故障番号
  vector<vector<const TpgFault*>> mDomCandListArray;

  // 被支配故障の候補リストの配列
  // キーは故障番号
  vector<vector<const TpgFault*>> mRevCandListArray;

};

inline
bool
operator==(
  const Reducer::Key& left,
  const Reducer::Key& right
)
{
  return left.fault_id == right.fault_id &&
    left.ffr_id == right.ffr_id;
}

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

template<>
struct hash<DRUID_NAMESPACE::Reducer::Key>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::Reducer::Key& x
  ) const
  {
    return x.fault_id * 1023 + x.ffr_id;
  }
};

END_NAMESPACE_STD

#endif // REDUCER_H
