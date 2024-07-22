#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultStatus.h"
#include "NodeTimeValList.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief 故障の情報を表すクラス
///
/// * 十分割り当てを持つ．
/// * 場合によっては必要割り当ても持つ．
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief ネットワーク全体に対して故障情報の生成を行う．
  /// @return 故障情報の配列を返す．
  ///
  /// - 配列のキーは故障番号
  /// - fault_list に含まれない故障の情報は不定
  static
  vector<FaultInfo>
  generate(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障リスト
    const JsonValue& opton = JsonValue{}       ///< [in] オプション
  );

  /// @brief 空のコンストラクタ
  FaultInfo() = default;

  /// @brief デストラクタ
  ~FaultInfo() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を返す．
  const TpgFault*
  fault() const
  {
    return mFault;
  }

  /// @brief 故障をセットする．
  void
  set_fault(
    const TpgFault* fault
  )
  {
    mFault = fault;
  }

  /// @brief 故障の状態を返す．
  FaultStatus
  status() const
  {
    if ( mFlags[0] ) {
      return FaultStatus::Detected;
    }
    if ( mFlags[1] ) {
      return FaultStatus::Untestable;
    }
    return FaultStatus::Undetected;
  }

  /// @brief 十分割り当てを返す．
  const NodeTimeValList&
  sufficient_condition() const
  {
    return mSuffCond;
  }

  /// @brief 外部入力の割り当てを返す．
  const NodeTimeValList&
  pi_assign() const
  {
    return mPiAssign;
  }

  /// @brief 必要割り当てが計算済みから調べる．
  bool
  has_mandatory_condition() const
  {
    return mFlags[2];
  }

  /// @brief 必要割り当てを返す．
  ///
  /// has_mandatory_condition() == true の時のみ意味を持つ．
  const NodeTimeValList&
  mandatory_condition() const
  {
    return mMandCond;
  }

  /// @brief 検出条件が単一のキューブの時 true を返す．
  bool
  is_trivial() const
  {
    return mFlags[3];
  }

  /// @brief 十分割り当てをセットする．
  ///
  /// 自動的に status が detected になる．
  void
  set_sufficient_condition(
    const NodeTimeValList& suff_cond,
    const NodeTimeValList& pi_assign
  )
  {
    mSuffCond = suff_cond;
    mPiAssign = pi_assign;
    mFlags[0] = true;
    mFlags[1] = false; // 念のため
    mFlags[4] = false;
  }

  /// @brief 必要割り当てをセットする．
  void
  set_mandatory_condition(
    const NodeTimeValList& mand_cond
  )
  {
    mMandCond = mand_cond;
    mFlags[2] = true;
    if ( compare(mSuffCond, mMandCond) == 3 ) {
      mFlags[3] = true;
    }
  }

  /// @brief テスト不能故障の印をつける．
  void
  set_untestable()
  {
    mFlags[0] = false; // 念のため
    mFlags[1] = true;
  }

  /// @brief 削除フラグを調べる．
  bool
  is_deleted() const
  {
    return mFlags[4];
  }

  /// @brief 削除の印をつける．
  void
  set_deleted()
  {
    mFlags[4] = true;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  const TpgFault* mFault{nullptr};

  // いくつかの情報をパックしたもの
  // 0: 検出済み
  // 1: テスト不能
  // 2: mMandCond 計算済み
  // 3: trivial フラグ
  // 4: 削除フラグ
  bitset<5> mFlags{16U};

  // 十分割り当て
  NodeTimeValList mSuffCond;

  // テストパタン用の割り当て
  NodeTimeValList mPiAssign;

  // 必要割り当て
  NodeTimeValList mMandCond;

};

END_NAMESPACE_DRUID

#endif // FAULTINFO_H
