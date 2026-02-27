
/// @file ResultRep.cc
/// @brief ResultRep とその派生クラスの実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "ResultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ResultRep
//////////////////////////////////////////////////////////////////////

// 値割り当てを返す．
const AssignList&
ResultRep::assign_list() const
{
  throw std::logic_error{"No AssignList"};
}

// テストベクタを返す．
const TestVector&
ResultRep::testvector() const
{
  throw std::logic_error{"No TestVector"};
}


//////////////////////////////////////////////////////////////////////
// クラス ResultRep_DT
//////////////////////////////////////////////////////////////////////

// @brief 複製を作る．
ResultRep*
ResultRep_DT::duplicate() const
{
  return new ResultRep_DT(mAssignList, mTestVector);
}

// 結果を返す．
FaultStatus
ResultRep_DT::status() const
{
  return FaultStatus::Detected;
}

// 値割り当てを返す．
const AssignList&
ResultRep_DT::assign_list() const
{
  return mAssignList;
}

// テストベクタを返す．
const TestVector&
ResultRep_DT::testvector() const
{
  return mTestVector;
}


//////////////////////////////////////////////////////////////////////
// クラス ResultRep_UT
//////////////////////////////////////////////////////////////////////

// @brief 複製を作る．
ResultRep*
ResultRep_UT::duplicate() const
{
  return new ResultRep_UT();
}

// 結果を返す．
FaultStatus
ResultRep_UT::status() const
{
  return FaultStatus::Untestable;
}

END_NAMESPACE_DRUID
