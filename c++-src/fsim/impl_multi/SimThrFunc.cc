
/// @file SimThrFunc.cc
/// @brief SimThrFunc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimThrFunc.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// クラス SimThrFunc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SimThrFunc::SimThrFunc(
  SizeType id,
  const TpgNetwork& network,
  const TpgFaultList& fault_list
) : mId{id},
    mEngine(network, fault_list)
{
}

// @brief デストラクタ
SimThrFunc::~SimThrFunc()
{
}

// @brief SPPFP 法のシミュレーションを行う．
std::vector<SizeType>
SimThrFunc::sppfp(
  const TestVector& tv
)
{
  mEngine.calc_val(tv);
  return mEngine.sppfp();
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<std::vector<SizeType>>& det_list_array
)
{
  auto ntv = end - begin;

  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    auto det_list = mEngine.sppfp();
    // 結果を追加する．
    det_list_array[index] = det_list;
  }
}

// @brief SPPFP 法のシミュレーションを行う．
std::vector<SizeType>
SimThrFunc::sppfp(
  const AssignList& assign_list
)
{
  mEngine.calc_val(assign_list);
  return mEngine.sppfp();
}

void
SimThrFunc::ppsfp(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<std::vector<SizeType>>& det_list_array
)
{
  auto ntv = end - begin;

  // PV_BITLEN ごとに分割して処理を行う．
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = begin;
  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    tv_buff.push_back(tv);
    auto buff_size = tv_buff.size();
    if ( buff_size == PV_BITLEN || index == end - 1 )  {
      // 正常値の計算を行う．
      mEngine.calc_val(tv_buff);
      // パタン並列シュミレーションを行う．
      auto det_list_array1 = mEngine.ppsfp(buff_size);
      // 結果を追加する．
      for ( SizeType i = 0; i < buff_size; ++ i ) {
	det_list_array[i + base] = det_list_array1[i];
      }
      base += buff_size;
      tv_buff.clear();
    }
  }
}

// @brief SPPFP 法のシミュレーションを行う．
FsimResultsRep*
SimThrFunc::sppfp2(
  const TestVector& tv
)
{
  mEngine.calc_val(tv);
  return mEngine.sppfp2();
}

// @brief PPSFP2 法のシミュレーションを行う．
void
SimThrFunc::sppfp2(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FsimResultsRep*>& res_array
)
{
  auto ntv = end - begin;

  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    auto res = mEngine.sppfp2();
    // 結果を追加する．
    res_array[index] = res;
  }
}

// @brief SPPFP 法のシミュレーションを行う．
FsimResultsRep*
SimThrFunc::sppfp2(
  const AssignList& assign_list
)
{
  mEngine.calc_val(assign_list);
  return mEngine.sppfp2();
}

void
SimThrFunc::ppsfp2(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FsimResultsRep*>& res_array
)
{
  auto ntv = end - begin;

  // PV_BITLEN ごとに分割して処理を行う．
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = begin;
  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    tv_buff.push_back(tv);
    auto buff_size = tv_buff.size();
    if ( buff_size == PV_BITLEN || index == end - 1 )  {
      // 正常値の計算を行う．
      mEngine.calc_val(tv_buff);
      // パタン並列シュミレーションを行う．
      auto res_array1 = mEngine.ppsfp2(buff_size);
      // 結果を追加する．
      for ( SizeType i = 0; i < buff_size; ++ i ) {
	res_array[i + base] = res_array1[i];
      }
      base += buff_size;
      tv_buff.clear();
    }
  }
}

END_NAMESPACE_DRUID_FSIM
