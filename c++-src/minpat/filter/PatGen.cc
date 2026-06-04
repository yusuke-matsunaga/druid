
/// @file PatGen.cc
/// @brief PatGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PatGen.h"
#include "RandPatGen.h"
#include "Tv1PatGen.h"
//#include "Tv2PatGen.h"
#include "fsim/FsimResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス PatGen
//////////////////////////////////////////////////////////////////////

// @brief 新しいオブジェクトを作る．
std::unique_ptr<PatGen>
PatGen::new_obj(
  const FaultInfo& fault_info,
  const ConfigParam& option
)
{
  auto str = option.get_string_elem("method", "random");
  PatGen* obj = nullptr;
  if ( str == "random" ) {
    obj = new RandPatGen(fault_info);
  }
  else if ( str == "tv1" ) {
    obj = new Tv1PatGen(fault_info);
  }
#if 0
  else if ( str == "tv2" ) {
    obj = new Tv2PatGen(fault_info);
  }
#endif
  else {
    std::ostringstream buf;
    buf << str << ": unknown option for 'method'";
    throw std::invalid_argument{buf.str()};
  }
  return std::unique_ptr<PatGen>{obj};
}


//////////////////////////////////////////////////////////////////////
// RandPatGen
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
RandPatGen::RandPatGen(
  const FaultInfo& fault_info
) : PatGen(fault_info)
{
}

// @brief パタンを作る．
void
RandPatGen::gen(
  SizeType size,
  std::vector<TestVector>& tv_buff
)
{
  for ( SizeType i = 0; i < size; ++ i ) {
    auto tv = TestVector(network());
    tv.set_from_random(mRandGen);
    tv_buff[i] = tv;
  }
}

// @brief 検出結果で更新する．
void
RandPatGen::update(
  const FsimResults& res
)
{
  // なにもしない．
}


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

#if 0
//////////////////////////////////////////////////////////////////////
// クラス Tv2PatGen
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Tv2PatGen::Tv2PatGen(
  const FaultInfo& fault_info
) : PatGen(fault_info)
{
  for ( auto fault: fault_info.fault_list() ) {
    mHeap.put_item({fault, 0});
  }
}

// @brief パタンを作る．
void
Tv2PatGen::gen(
  SizeType size,
  std::vector<TestVector>& tv_buff
)
{
  for ( SizeType i = 0; i < size; ++ i ) {
  }
}

// @brief 検出結果で更新する．
void
Tv2PatGen::update(
  const FsimResults& res
)
{
}
#endif

END_NAMESPACE_DRUID
