
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

// テストベクタを持つ時 true を返す．
bool
ResultRep::has_testvector() const
{
  return false;
}

// テストベクタを返す．
const TestVector&
ResultRep::testvector() const
{
  throw std::logic_error{"No TestVector"};
}

// 値割り当てを持つ時 true を返す．
bool
ResultRep::has_assign_list() const
{
  return false;
}

// 値割り当てを返す．
const AssignList&
ResultRep::assign_list() const
{
  throw std::logic_error{"No AssignList"};
}


//////////////////////////////////////////////////////////////////////
// クラス ResultRep_TV
//////////////////////////////////////////////////////////////////////

// @brief 複製を作る．
ResultRep*
ResultRep_TV::duplicate() const
{
  return new ResultRep_TV(mTestVector);
}

// 結果を返す．
FaultStatus
ResultRep_TV::status() const
{
  return FaultStatus::Detected;
}

// テストベクタを持つ時 true を返す．
bool
ResultRep_TV::has_testvector() const
{
  return true;
}

// テストベクタを返す．
const TestVector&
ResultRep_TV::testvector() const
{
  return mTestVector;
}


//////////////////////////////////////////////////////////////////////
// クラス ResultRep_AL
//////////////////////////////////////////////////////////////////////

// @brief 複製を作る．
ResultRep*
ResultRep_AL::duplicate() const
{
  return new ResultRep_AL(mAssignList);
}

// 結果を返す．
FaultStatus
ResultRep_AL::status() const
{
  return FaultStatus::Detected;
}

// 値割り当てを持つ時 true を返す．
bool
ResultRep_AL::has_assign_list() const
{
  return true;
}

// 値割り当てを返す．
const AssignList&
ResultRep_AL::assign_list() const
{
  return mAssignList;
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
