
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FsimX.h"

#include "types/TpgNetwork.h"
#include "types/TpgNode.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"

#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

std::unique_ptr<FsimImpl>
new_Fsim(
  const TpgFaultList& fault_list
)
{
  return std::unique_ptr<FsimImpl>{new FSIM_CLASSNAME(fault_list)};
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME(
  const TpgFaultList& fault_list
) : mEngine(fault_list.network(), fault_list)
{
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  mEngine.set_skip(true);
}

// @brief 故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip(
  SizeType fid
)
{
  mEngine.set_skip(fid);
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  mEngine.clear_skip_all();
}

// @brief 故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip(
  SizeType fid
)
{
  mEngine.clear_skip(fid);
}

// @brief 故障のスキップマークを得る．
bool
FSIM_CLASSNAME::get_skip(
  SizeType fid
) const
{
  return mEngine.get_skip(fid);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const TestVector& tv,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  return mEngine.spsfp(fid);
}

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::spsfp(
  const AssignList& assign_list,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  return mEngine.spsfp(fid);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const TestVector& tv,
  FidList& fid_list

)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  mEngine.sppfp(fid_list);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const AssignList& assign_list,
  FidList& fid_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  mEngine.sppfp(fid_list);
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp(
  const std::vector<TestVector>& tv_list,
  std::vector<FidList>& fid_list_array
)
{
  auto ntv = tv_list.size();
  for ( SizeType i = 0; i < ntv; ++ i ) {
    auto& tv = tv_list[i];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    mEngine.sppfp(fid_list_array[i]);
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::ppsfp(
  const std::vector<TestVector>& tv_list,
  std::vector<FidList>& fid_list_array
)
{
  auto ntv = tv_list.size();

  // PV_BITLEN ごとに分割して処理を行う．
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    auto buff_size = tv_buff.size();
    if ( buff_size == PV_BITLEN || buff_size + base == ntv )  {
      // 正常値の計算を行う．
      mEngine.calc_val(tv_buff);
      // パタン並列シュミレーションを行う．
      mEngine.ppsfp(buff_size, fid_list_array, base);
      base += buff_size;
      tv_buff.clear();
    }
  }
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
FSIM_CLASSNAME::spsfp2(
  const TestVector& tv,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  return mEngine.spsfp2(fid);
}

// @brief SPSFP故障シミュレーションを行う．
DiffBits
FSIM_CLASSNAME::spsfp2(
  const AssignList& assign_list,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  return mEngine.spsfp2(fid);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp2(
  const TestVector& tv,
  FidList& fid_list,
  DiffBitsDict& dbits_dict
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  mEngine.sppfp2(fid_list, dbits_dict);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp2(
  const AssignList& assign_list,
  FidList& fid_list,
  DiffBitsDict& dbits_dict
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  mEngine.sppfp2(fid_list, dbits_dict);
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::sppfp2(
  const std::vector<TestVector>& tv_list,
  std::vector<FidList>& fid_list_array,
  std::vector<DiffBitsDict>& dbits_dict_array
)
{
  auto ntv = tv_list.size();
  for ( SizeType i = 0; i < ntv; ++ i ) {
    auto& tv = tv_list[i];
    // 正常値の計算を行う．
    mEngine.calc_val(tv);
    // 故障シュミレーションを行う．
    mEngine.sppfp2(fid_list_array[i], dbits_dict_array[i]);
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
FSIM_CLASSNAME::ppsfp2(
  const std::vector<TestVector>& tv_list,
  std::vector<FidList>& fid_list_array,
  std::vector<DiffBitsDict>& dbits_dict_array
)
{
  auto ntv = tv_list.size();

  // PV_BITLEN ごとに分割して処理を行う．
  std::vector<TestVector> tv_buff;
  tv_buff.reserve(PV_BITLEN);
  SizeType base = 0;
  for ( auto& tv: tv_list ) {
    tv_buff.push_back(tv);
    auto buff_size = tv_buff.size();
    if ( buff_size == PV_BITLEN || buff_size + base == ntv )  {
      // 正常値の計算を行う．
      mEngine.calc_val(tv_buff);
      // パタン並列シュミレーションを行う．
      mEngine.ppsfp2(buff_size, fid_list_array, dbits_dict_array, base);
      base += buff_size;
      tv_buff.clear();
    }
  }
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
#if FSIM_COMBI
  return 0;
#endif
#if FSIM_BSIDE
  return mEngine.calc_wsa(i_vect, weighted);
#endif
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
FSIM_CLASSNAME::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
#if FSIM_COMBI
  return 0;
#endif
#if FSIM_BSIDE
  return mEngine.calc_wsa(tv, weighted);
#endif
}

// @brief 状態を設定する．
void
FSIM_CLASSNAME::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
#if FSIM_BSIDE
  mEngine.set_state(i_vect, f_vect);
#endif
}

// @brief 状態を取得する．
void
FSIM_CLASSNAME::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
#if FSIM_BSIDE
  mEngine.get_state(i_vect, f_vect);
#endif
}

END_NAMESPACE_DRUID_FSIM
