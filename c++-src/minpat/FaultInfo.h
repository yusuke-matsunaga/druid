#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeTimeValList.h"



BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief 故障に関する情報を保持するクラス
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief コンストラクタ
  FaultInfo(
    const TpgFault* fault,            ///< [in] 対象の故障
    const NodeTimeValList& mand_cond, ///< [in] 必要条件
    const NodeTimeValList& suff_cond  ///< [in] 十分条件の一つ
  ) : mFault{fault},
      mMandCond{mand_cond},
      mSuffCondList{suff_cond}
  {
    mTrivial = compare(mand_cond, suff_cond) == 3;
  }

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

  /// @brief 必要条件を返す．
  const NodeTimeValList&
  mandatory_condition() const
  {
    return mMandCond;
  }

  /// @brief 十分条件のリストを返す．
  const vector<NodeTimeValList>&
  sufficient_conditions() const
  {
    return mSuffCondList;
  }

  /// @brief 必要条件が十分条件となっているとき true を返す．
  bool
  is_trivial() const
  {
    return mTrivial;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  const TpgFault* mFault;

  // 必要条件
  NodeTimeValList mMandCond;

  // 十分条件のリスト
  vector<NodeTimeValList> mSuffCondList;

  // trivial フラグ
  bool mTrivial;

};

END_NAMESPACE_DRUID

#endif // FAULTINFO_H
