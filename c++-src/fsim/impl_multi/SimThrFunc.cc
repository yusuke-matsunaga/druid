
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
void
SimThrFunc::sppfp(
  const TestVector& tv,
  FidList& fid_list
)
{
  mEngine.calc_val(tv);
  mEngine.sppfp(fid_list);
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp(
  const AssignList& assign_list,
  FidList& fid_list
)
{
  mEngine.calc_val(assign_list);
  mEngine.sppfp(fid_list);
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FidList>& det_list_array
)
{
  auto ntv = end - begin;

  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    mEngine.sppfp(det_list_array[index]);
  }
}

void
SimThrFunc::ppsfp(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FidList>& det_list_array
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
      mEngine.ppsfp(buff_size, det_list_array, base);
      base += buff_size;
      tv_buff.clear();
    }
  }
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp2(
  const TestVector& tv,
  FidList& fid_list,
  DiffBitsDict& dbits_dict
)
{
  mEngine.calc_val(tv);
  mEngine.sppfp2(fid_list, dbits_dict);
}

// @brief SPPFP 法のシミュレーションを行う．
void
SimThrFunc::sppfp2(
  const AssignList& assign_list,
  FidList& fid_list,
  DiffBitsDict& dbits_dict
)
{
  mEngine.calc_val(assign_list);
  mEngine.sppfp2(fid_list, dbits_dict);
}

// @brief PPSFP2 法のシミュレーションを行う．
void
SimThrFunc::sppfp2(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FidList>& fid_list_array,
  std::vector<DiffBitsDict>& dbits_dict_array
)
{
  for ( SizeType index = begin; index < end; ++ index ) {
    auto& tv = tv_list[index];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    mEngine.sppfp2(fid_list_array[index], dbits_dict_array[index]);
  }
}

void
SimThrFunc::ppsfp2(
  const std::vector<TestVector>& tv_list,
  SizeType begin,
  SizeType end,
  std::vector<FidList>& fid_list_array,
  std::vector<DiffBitsDict>& dbits_dict_array
)
{
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
      mEngine.ppsfp2(buff_size, fid_list_array, dbits_dict_array, base);
      base += buff_size;
      tv_buff.clear();
    }
  }
}

END_NAMESPACE_DRUID_FSIM
