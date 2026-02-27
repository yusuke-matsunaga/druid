
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

// @brief 空のコンストラクタ
DtpgResults::DtpgResults()
{
  clear();
}

// @brief クリアする．
void
DtpgResults::clear()
{
  for ( auto& p: mResultDict ) {
    delete p.second;
  }
  mResultDict.clear();

  mDetCount = 0;
  mDetTime = 0.0;

  mUntestCount = 0;
  mUntestTime = 0.0;

  mAbortCount = 0;
  mAbortTime = 0.0;

  mCnfGenCount = 0;
  mCnfGenTime = 0.0;

  mSatStats.clear();
  mSatStatsMax.clear();

  mBackTraceTime = 0.0;
}

/// @brief 検出済みに設定する．
void
DtpgResults::set_detected(
  const TpgFault& fault,
  const AssignList& assign_list,
  const TestVector& testvect
)
{
  if ( mResultDict.count(fault.id()) > 0 ) {
    std::ostringstream buf;
    buf << fault.str() << " has already set";
    throw std::invalid_argument{buf.str()};
  }
  auto r = new ResultRep_DT(assign_list, testvect);
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

// @brief 内容をマージする．
void
DtpgResults::merge(
  const DtpgResults& src
)
{
  for ( auto& p: src.mResultDict ) {
    auto fid = p.first;
    auto src_rep = p.second;
    auto rep = src_rep->duplicate();
    mResultDict.erase(fid);
    mResultDict.emplace(fid, rep);
  }
  mDetCount += src.mDetCount;
  mDetTime += src.mDetTime;
  mUntestCount += src.mUntestCount;
  mUntestTime += src.mUntestTime;
  mAbortCount += src.mAbortCount;
  mAbortTime += src.mAbortTime;
  mCnfGenCount += src.mCnfGenCount;
  mCnfGenTime += src.mCnfGenTime;
  mSatStats += src.mSatStats;
  mSatStatsMax.max_assign(src.mSatStatsMax);
  mBackTraceTime += src.mBackTraceTime;
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

// @brief 値割り当てを返す．
const AssignList&
DtpgResults::assign_list(
  const TpgFault& fault
) const
{
  auto r = mResultDict.at(fault.id());
  return r->assign_list();
}

// @brief テストベクタを返す．
const TestVector&
DtpgResults::testvector(
  const TpgFault& fault
) const
{
  auto r = mResultDict.at(fault.id());
  return r->testvector();
}

END_NAMESPACE_DRUID
