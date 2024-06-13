#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeValList.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief MinPat 用の故障情報を表すクラス
///
/// 以下の情報を持つ．
/// - 故障
/// - 検出のための必要条件
/// - 検出のための十分条件
/// - テストベクタ
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief コンストラクタ
  FaultInfo(
    const TpgFault* fault,        ///< [in] 対象の故障
    const NodeValList& mand_cond, ///< [in] 必要条件
    const NodeValList& suff_cond, ///< [in] 十分条件
    const TestVector& testvect	  ///< [in] テストベクタ
  ) : mFault{fault},
      mMandCond{mand_cond},
      mSufficientCond{suff_cond},
      mTestVector{testvect}
  {
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
  const NodeValList&
  mandatory_condition() const
  {
    return mMandCond;
  }

  /// @brief 十分条件を返す．
  const NodeValList&
  sufficient_condition() const
  {
    return mSufficientCond;
  }

  /// @brief テストベクタを返す．
  const TestVector&
  testvect() const
  {
    reutrn mTestVector;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障
  const TpgFault* mFault;

  // 必要条件
  NodeValList mMandCond;

  // 十分条件
  NodeValList mSufficientCond;

  // テストベクタ
  TestVector mTestVector;

};

END_NAMESPACE_DRUID

#endif // FAULTINFO_H
