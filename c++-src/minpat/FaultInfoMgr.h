#ifndef FAULTINFOMGR_H
#define FAULTINFOMGR_H

/// @file FaultInfoMgr.h
/// @brief FaultInfoMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FFRFaultList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class TestCover;

//////////////////////////////////////////////////////////////////////
/// @class FaultInfoMgr FaultInfoMgr.h "FaultInfoMgr.h"
/// @brief FaultInfo を管理するクラス
//////////////////////////////////////////////////////////////////////
class FaultInfoMgr
{
public:

  /// @brief コンストラクタ
  FaultInfoMgr(
    const TpgNetwork& network,                ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list ///< [in] 対象の故障リスト
  );

  /// @brief デストラクタ
  ~FaultInfoMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 故障を持つFFRのリストを返す．
  const vector<const TpgFFR*>&
  ffr_list() const
  {
    return mFFRFaultList.ffr_list();
  }

  /// @brief 故障リストを返す．
  const vector<const TpgFault*>&
  fault_list() const
  {
    return mFaultList;
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
    return mFaultInfoArray[fault->id()];
  }

  /// @brief 故障情報を返す．
  FaultInfo&
  fault_info(
    const TpgFault* fault ///< [in] 故障
  )
  {
    return mFaultInfoArray[fault->id()];
  }

  /// @brief 故障情報を求める．
  void
  generate(
    const JsonValue& option ///< [in] オプション
                            ///<      - 'sat_param': object SATソルバ用のパラメータ
                            ///<      - 'justifier': str    justifier用のパラメータ
                            ///<      - 'debug': bool         デバッグフラグ
  );

  /// @brief 支配関係を用いて削除マークをつける．
  ///
  /// generate() で故障情報を作成していなければなにもしない．
  void
  reduce(
    const JsonValue& option ///< [in] オプション
                            ///<      - 'sat_param': object SATソルバ用のパラメータ
                            ///<      - 'loop_limit': int   シミュレーション回数
                            ///<      - 'no_analyze': bool  制御フラグ
                            ///<      - 'debug': bool       デバッグフラグ
  );

  /// @brief 残った故障に対してテストカバーを作る．
  vector<TestCover>
  gen_cover(
    const JsonValue& option ///< [in] オプション
                            ///<      - 'sat_param': object   SATソルバ用のパラメータ
                            ///<      - 'cube_per_fault': int 1故障あたりのキューブ数
                            ///<      - 'debug': bool         デバッグフラグ
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
    bool debug      ///< [in] デバッグフラグ
  );

  /// @brief 同一FFR内の支配関係を用いて故障を削減する．
  void
  ffr_reduction(
    const JsonValue& option ///< [in] オプション
  );

  /// @brief 故障の解析を行う．
  ///
  /// 結果は FaultInfo.mSuffCond, mMandCond に格納される．
  void
  fault_analysis(
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

  /// @brief fault が削除されているか調べる．
  bool
  is_deleted(
    const TpgFault* fault
  ) const
  {
    return fault_info(fault).is_deleted();
  }

  /// @brief fault に削除された印をつける．
  void
  set_deleted(
    const TpgFault* fault
  )
  {
    fault_info(fault).set_deleted();
    -- mFaultNum;
  }

  /// @brief trivial fault か調べる．
  bool
  is_trivial(
    const TpgFault* fault
  )
  {
    return fault_info(fault).is_trivial();
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

  /// @brief オプションからデバッグフラグを取り出す．
  static
  bool
  get_debug(
    const JsonValue& option
  )
  {
    if ( option.is_object() && option.has_key("debug") ) {
      return option.get("debug").get_bool();
    }
    return false;
  }


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

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障リスト
  // テスト不能故障，未検出故障も含む
  vector<const TpgFault*> mFaultList;

  // 現時点で処理対象の故障のリスト
  vector<const TpgFault*> mActiveFaultList;

  // 残っている故障数
  SizeType mFaultNum{0};

  // FFR 単位で故障を管理するオブジェクト
  FFRFaultList mFFRFaultList;

  // FaultInfo の配列
  // キーは故障番号
  vector<FaultInfo> mFaultInfoArray;

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

END_NAMESPACE_DRUID

BEGIN_NAMESPACE_STD

template<>
struct hash<DRUID_NAMESPACE::FaultInfoMgr::Key>
{
  SizeType
  operator()(
    const DRUID_NAMESPACE::FaultInfoMgr::Key& x
  ) const
  {
    return x.fault_id * 1023 + x.ffr_id;
  }
};

END_NAMESPACE_STD

#endif // FAULTINFOMGR_H
