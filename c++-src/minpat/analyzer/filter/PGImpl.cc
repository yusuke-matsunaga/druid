
/// @file PGImpl.cc
/// @brief PGImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PGImpl.h"
#include "PGImpl_Random.h"
//#include "Tv1PatGen.h"
#include "PGImpl_TV.h"
#include "fsim/FsimResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// PGImpl_Random
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PGImpl_Random::PGImpl_Random(
  const FaultInfo& fault_info
) : PGImpl(fault_info)
{
}

// @brief パタンを作る．
bool
PGImpl_Random::get_pat(
  TestVector& tv
)
{
  tv = TestVector(network());
  tv.set_from_random(mRandGen);
  return true;
}

// @brief 検出結果で更新する．
void
PGImpl_Random::update(
  const FsimResults& res
)
{
  // なにもしない．
}

#if 0
//////////////////////////////////////////////////////////////////////
// Tv1PatGen
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Tv1PatGen::Tv1PatGen(
  const FaultInfo& fault_info
) : PatGen(fault_info),
    mUndetList{fault_info.rep_fault_list()}
{
}

// @brief パタンを作る．
void
Tv1PatGen::gen(
  SizeType size,
  std::vector<TestVector>& tv_buff
)
{
  SizeType j = 0;
  for ( SizeType i = 0; i < size; ++ i ) {
    if ( j < mUndetList.size() ) {
      auto fault = mUndetList[j];
      ++ j;
      auto tv = fault_info().testvector(fault);
      tv_buff[i] = tv;
    }
    else {
      // ランダムパタンを用いる．
      auto tv = TestVector(network());
      tv.set_from_random(mRandGen);
      tv_buff[i] = tv;
    }
  }
}

// @brief 検出結果で更新する．
void
Tv1PatGen::update(
  const FsimResults& res
)
{
  std::vector<bool> mark(network().max_fault_id(), false);
  auto ntv = res.tv_num();
  for ( SizeType i = 0; i < ntv; ++ i ) {
    for ( auto fault: res.fault_list(i) ) {
      mark[fault.id()] = true;
    }
  }
  TpgFaultList new_list;
  new_list.reserve(mUndetList.size());
  for ( auto fault: mUndetList ) {
    if ( mark[fault.id()] ) {
      continue;
    }
    new_list.push_back(fault);
  }
  if ( new_list.size() != mUndetList.size() ) {
    std::swap(new_list, mUndetList);
  }
}
#endif

//////////////////////////////////////////////////////////////////////
// クラス PGImpl_TV
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PGImpl_TV::PGImpl_TV(
  const FaultInfo& fault_info
) : PGImpl(fault_info),
    mFaultList{fault_list()},
    mNext{0}
{
}

// @brief パタンを作る．
bool
PGImpl_TV::get_pat(
  TestVector& tv
)
{
  if ( mNext < mFaultList.size() ) {
    auto fault = mFaultList[mNext];
    ++ mNext;
    tv = fault_info().testvector(fault);
    return true;
  }
  return false;
}

// @brief 検出結果で更新する．
void
PGImpl_TV::update(
  const FsimResults& res
)
{
  // なにもしない．
}

END_NAMESPACE_DRUID
