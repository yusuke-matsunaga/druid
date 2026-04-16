
/// @file DtpgResults.cc
/// @brief DtpgResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgResults.h"
#include "types/TpgFault.h"
#include "ResultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DtpgResults
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
DtpgResults::DtpgResults(
  SizeType size
) : mResultArray(size)
{
  clear();
}

// @brief コピーコンストラクタ
DtpgResults::DtpgResults(
  const DtpgResults& src
) : mResultArray(src.mResultArray.size())
{
  auto n = mResultArray.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto src_rep = src.mResultArray[i].get();
    if ( src_rep == nullptr ) {
      continue;
    }
    auto rep = src_rep->duplicate();
    mResultArray[i] = std::unique_ptr<ResultRep>{rep};
  }
  mStats = src.mStats;
}

// @brief デストラクタ
DtpgResults::~DtpgResults()
{
}

// @brief クリアする．
void
DtpgResults::clear()
{
  auto n = mResultArray.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    mResultArray[i] = nullptr;
  }
  mStats.clear();
}

/// @brief 検出済みに設定する．
void
DtpgResults::set_detected(
  const TpgFault& fault,
  const SuffCond& cond,
  const TestVector& testvect
)
{
  _check_result(fault);
  auto r = new ResultRep_DT(cond, testvect);
  mResultArray[fault.id()] = std::unique_ptr<ResultRep>{r};
}

// @brief テスト不能に設定する．
void
DtpgResults::set_untestable(
  const TpgFault& fault
)
{
  _check_result(fault);
  auto r = new ResultRep_UT();
  mResultArray[fault.id()] = std::unique_ptr<ResultRep>{r};
}

// @brief 統計情報を更新する．
void
DtpgResults::merge_stats(
  const DtpgStats& stats
)
{
  mStats.merge(stats);
}

// @brief 結果を返す．
FaultStatus
DtpgResults::status(
  const TpgFault& fault
) const
{
  auto rep = mResultArray[fault.id()].get();
  if ( rep == nullptr ) {
    return FaultStatus::Undetected;
  }
  return rep->status();
}

// @brief 値割り当てを返す．
const SuffCond&
DtpgResults::cond(
  const TpgFault& fault
) const
{
  auto rep = mResultArray[fault.id()].get();
  return rep->cond();
}

// @brief テストベクタを返す．
const TestVector&
DtpgResults::testvector(
  const TpgFault& fault
) const
{
  auto rep = mResultArray[fault.id()].get();
  return rep->testvector();
}

// @brief 結果が未設定かチェックする．
void
DtpgResults::_check_result(
  const TpgFault& fault
) const
{
  if ( mResultArray[fault.id()].get() != nullptr ) {
    std::ostringstream buf;
    buf << fault.str() << " has already been set";
    std::cout << buf.str() << std::endl;
    throw std::invalid_argument{buf.str()};
  }
}

END_NAMESPACE_DRUID
