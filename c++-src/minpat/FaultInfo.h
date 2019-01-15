#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeValList.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief MinPat 用の故障情報を表すクラス
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief コンストラクタ
  /// @param[in] fault 対象の故障
  /// @param[in] mand_cond 必要条件
  /// @param[in] suff_cond 十分条件
  /// @param[in] testvect テストベクタ
  FaultInfo(const TpgFault* fault,
	    const NodeValList& mand_cond,
	    const NodeValList& suff_cond,
	    const TestVector& testvect);

  /// @brief デストラクタ
  ~FaultInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を返す．
  const TpgFault*
  fault() const;

  /// @brief 必要条件を返す．
  const NodeValList&
  mand_cond() const;

  /// @brief 十分条件を返す．
  const NodeValList&
  suff_cond() const;

  /// @brief テストベクタを返す．
  const TestVector&
  testvect() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault 対象の故障
// @param[in] mand_cond 必要条件
// @param[in] suff_cond 十分条件
// @param[in] testvect テストベクタ
inline
FaultInfo::FaultInfo(const TpgFault* fault,
		     const NodeValList& mand_cond,
		     const NodeValList& suff_cond,
		     const TestVector& testvect) :
  mFault(fault),
  mMandCond(mand_cond),
  mSufficientCond(suff_cond),
  mTestVector(testvect)
{
}

// @brief デストラクタ
inline
FaultInfo::~FaultInfo()
{
}

// @brief 故障を返す．
inline
const TpgFault*
FaultInfo::fault() const
{
  return mFault;
}

// @brief 必要条件を返す．
inline
const NodeValList&
FaultInfo::mand_cond() const
{
  return mMandCond;
}

// @brief 十分条件の割当リストを返す．
inline
const NodeValList&
FaultInfo::suff_cond() const
{
  return mSufficientCond;
}

// @brief テストベクタを返す．
inline
const TestVector&
FaultInfo::testvect() const
{
  return mTestVector;
}

END_NAMESPACE_DRUID

#endif // FAULTINFO_H
