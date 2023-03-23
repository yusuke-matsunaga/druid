
/// @file DopVerifyResult.cc
/// @brief DopVerifyResult の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopVerifyResult.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DopVerifyResult
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopVerifyResult::DopVerifyResult()
{
}

// @brief デストラクタ
DopVerifyResult::~DopVerifyResult()
{
}

// @brief 成功結果を追加する．
void
DopVerifyResult::add_good(
  const TpgFault& f
)
{
  mGoodList.push_back(f);
}

// @brief エラー結果を追加する．
void
DopVerifyResult::add_error(
  const TpgFault& f,
  const TestVector& tv
)
{
  mErrorList.push_back(ErrorCase(f, tv));
}

// @brief 成功回数を得る．
SizeType
DopVerifyResult::good_count() const
{
  return mGoodList.size();
}

// @brief 成功した故障を得る．
TpgFault
DopVerifyResult::good_fault(
  SizeType pos
) const
{
  ASSERT_COND( pos < good_count() );

  return mGoodList[pos];
}

// @brief エラー回数を得る．
SizeType
DopVerifyResult::error_count() const
{
  return mErrorList.size();
}

// @brief エラーの故障を得る．
TpgFault
DopVerifyResult::error_fault(
  SizeType pos
) const
{
  ASSERT_COND( pos < error_count() );

  return mErrorList[pos].mFault;
}

// @brief エラーとなったテストベクタを得る．
TestVector
DopVerifyResult::error_testvector(
  SizeType pos
) const
{
  ASSERT_COND( pos < error_count() );

  return mErrorList[pos].mTestVector;
}

DopVerifyResult::ErrorCase::ErrorCase(
  const TpgFault& f,
  const TestVector& tv
) : mFault{f},
    mTestVector{tv}
{
}

END_NAMESPACE_DRUID
