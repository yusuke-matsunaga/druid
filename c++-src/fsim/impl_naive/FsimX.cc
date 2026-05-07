
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

// @brief SPSFP故障シミュレーションを行う．
bool
FSIM_CLASSNAME::xspsfp(
  const AssignList& assign_list,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_valx(assign_list);

  // 故障伝搬を行う．
  auto res = mEngine.spsfp(fid);

  // init フラグを元に戻す．
  mEngine.clear_init();

  return res;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const TestVector& tv
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  return mEngine.sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::sppfp(
  const AssignList& assign_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  return mEngine.sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
std::vector<SizeType>
FSIM_CLASSNAME::xsppfp(
  const AssignList& assign_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_valx(assign_list);

  // 故障伝搬を行う．
  auto res = mEngine.sppfp();

  // init フラグを元に戻す．
  mEngine.clear_init();

  return res;
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<std::vector<SizeType>>
FSIM_CLASSNAME::ppsfp(
  const std::vector<TestVector>& tv_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv_list);

  // パタン数
  auto ntv = tv_list.size();
  return mEngine.ppsfp(ntv);
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

// @brief SPSFP故障シミュレーションを行う．
DiffBits
FSIM_CLASSNAME::xspsfp2(
  const AssignList& assign_list,
  SizeType fid
)
{
  // 正常値の計算を行う．
  mEngine.calc_valx(assign_list);

  // 故障伝搬を行う．
  auto res = mEngine.spsfp2(fid);

  // init フラグを元に戻す．
  mEngine.clear_init();

  return res;
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::sppfp2(
  const TestVector& tv
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv);

  // 故障伝搬を行う．
  return mEngine.sppfp2();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::sppfp2(
  const AssignList& assign_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(assign_list);

  // 故障伝搬を行う．
  return mEngine.sppfp2();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
FsimResultsRep*
FSIM_CLASSNAME::xsppfp2(
  const AssignList& assign_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_valx(assign_list);

  // 故障伝搬を行う．
  auto res = mEngine.sppfp2();

  // init フラグを元に戻す．
  mEngine.clear_init();

  return res;
}

// @brief 複数のパタンで故障シミュレーションを行う．
std::vector<FsimResultsRep*>
FSIM_CLASSNAME::ppsfp2(
  const std::vector<TestVector>& tv_list
)
{
  // 正常値の計算を行う．
  mEngine.calc_val(tv_list);

  // パタン数
  auto ntv = tv_list.size();

  return mEngine.ppsfp2(ntv);
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
