
/// @file DtpgResults.cc
/// @brief DtpgResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgResults.h"
#include "ResultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgResults
//////////////////////////////////////////////////////////////////////

// @brief クリアする．
void
DtpgResults::clear()
{
  for ( auto& p: mResultDict ) {
    delete p.second;
  }
  mResultDict.clear();
}

/// @brief 検出済みに設定する．
void
DtpgResults::set_detected(
  const TpgFault& fault,
  const TestVector& testvect
)
{
  if ( mResultDict.count(fault.id()) > 0 ) {
    std::ostringstream buf;
    buf << fault.str() << " has already set";
    throw std::invalid_argument{buf.str()};
  }
  auto r = new ResultRep_TV(testvect);
  mResultDict.emplace(fault.id(), r);
}

// @brief 検出済みに設定する．
void
DtpgResults::set_detected(
  const TpgFault& fault,
  const AssignList& assign_list
)
{
  if ( mResultDict.count(fault.id()) > 0 ) {
    std::ostringstream buf;
    buf << fault.str() << " has already set";
    throw std::invalid_argument{buf.str()};
  }
  auto r = new ResultRep_AL(assign_list);
  mResultDict.emplace(fault.id(), r);
}

// @brief テスト不能に設定する．
void
DtpgResults::set_untestable(
  const TpgFault& fault
)
{
  if ( mResultDict.count(fault.id()) > 0 ) {
    std::ostringstream buf;
    buf << fault.str() << " has already set";
    throw std::invalid_argument{buf.str()};
  }
  auto r = new ResultRep_UT();
  mResultDict.emplace(fault.id(), r);
}

// @brief 結果を返す．
FaultStatus
DtpgResults::status(
  const TpgFault& fault
) const
{
  if ( mResultDict.count(fault.id()) == 0 ) {
    return FaultStatus::Undetected;
  }
  auto r = mResultDict.at(fault.id());
  return r->status();
}

// @brief テストベクタを持つ時 true を返す．
bool
DtpgResults::has_testvector(
  const TpgFault& fault
) const
{
  if ( mResultDict.count(fault.id()) == 0 ) {
    return false;
  }
  auto r = mResultDict.at(fault.id());
  return r->has_testvector();
}

// @brief テストベクタを返す．
const TestVector&
DtpgResults::testvector(
  const TpgFault& fault
) const
{
  if ( !has_testvector(fault) ) {
    std::ostringstream buf;
    buf << fault.str() << "does not have a TestVector";
    throw std::logic_error{buf.str()};
  }
  auto r = mResultDict.at(fault.id());
  return r->testvector();
}

// @brief 値割り当てを持つ時 true を返す．
bool
DtpgResults::has_assign_list(
  const TpgFault& fault
) const
{
  if ( mResultDict.count(fault.id()) == 0 ) {
    return false;
  }
  auto r = mResultDict.at(fault.id());
  return r->has_assign_list();
}

// @brief 値割り当てを返す．
const AssignList&
DtpgResults::assign_list(
  const TpgFault& fault
) const
{
  if ( !has_assign_list(fault) ) {
    std::ostringstream buf;
    buf << fault.str() << "does not have an AssignList";
    throw std::logic_error{buf.str()};
  }
  auto r = mResultDict.at(fault.id());
  return r->assign_list();
}


//////////////////////////////////////////////////////////////////////
// クラス DtpgResults::ResultRep
//////////////////////////////////////////////////////////////////////

// テストベクタを持つ時 true を返す．
bool
DtpgResults::ResultRep::has_testvector() const
{
  return false;
}

// テストベクタを返す．
const TestVector&
DtpgResults::ResultRep::testvector() const
{
  throw std::logic_error{"No TestVector"};
}

// 値割り当てを持つ時 true を返す．
bool
DtpgResults::ResultRep::has_assign_list() const
{
  return false;
}

// 値割り当てを返す．
const AssignList&
DtpgResults::ResultRep::assign_list() const
{
  throw std::logic_error{"No AssignList"};
}


//////////////////////////////////////////////////////////////////////
// クラス ResultRep_TV
//////////////////////////////////////////////////////////////////////

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

// 結果を返す．
FaultStatus
ResultRep_UT::status() const
{
  return FaultStatus::Untestable;
}

END_NAMESPACE_DRUID
