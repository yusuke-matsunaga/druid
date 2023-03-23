
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Fsim.h"
#include "FsimImpl.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

namespace nsFsimSa2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network,
    TpgFaultMgr& fmgr
  );
}

namespace nsFsimSa3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network,
    TpgFaultMgr& fmgr
  );
}

namespace nsFsimTd2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network,
    TpgFaultMgr& fmgr
  );
}

namespace nsFsimTd3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network,
    TpgFaultMgr& fmgr
  );
}

BEGIN_NONAMESPACE

inline
std::unique_ptr<FsimImpl>
new_impl(
  const TpgNetwork& network,
  TpgFaultMgr& fmgr,
  bool has_x
)
{
  auto fault_type = fmgr.fault_type();
  if ( has_x ) {
    // 3値バージョン
    if ( fault_type == FaultType::StuckAt ||
	 fault_type == FaultType::GateExaustive ) {
      return nsFsimSa3::new_Fsim(network, fmgr);
    }
    if ( fault_type == FaultType::TransitionDelay ) {
      return nsFsimTd3::new_Fsim(network, fmgr);
    }
  }
  else {
    // 2値バージョン
    if ( fault_type == FaultType::StuckAt ||
	 fault_type == FaultType::GateExaustive ) {
      return nsFsimSa2::new_Fsim(network, fmgr);
    }
    if ( fault_type == FaultType::TransitionDelay ) {
      return nsFsimTd2::new_Fsim(network, fmgr);
    }
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Fsim::Fsim()
{
}

// @brief デストラクタ
Fsim::~Fsim()
{
}

// @brief 初期化
void
Fsim::initialize(
  const TpgNetwork& network,
  TpgFaultMgr& fmgr,
  bool has_x
)
{
  mImpl = new_impl(network, fmgr, has_x);
}

// @brief 全ての故障にスキップマークをつける．
void
Fsim::set_skip_all()
{
  mImpl->set_skip_all();
}

// @brief 故障にスキップマークをつける．
void
Fsim::set_skip(
  const TpgFault& f
)
{
  mImpl->set_skip(f);
}

// @brief 複数の故障にスキップマークをつける．
void
Fsim::set_skip(
  const TpgFaultList& fault_list
)
{
  clear_skip_all();
  for ( auto f: fault_list ) {
    set_skip(f);
  }
}

// @brief 複数の故障にスキップマークをつける．
void
Fsim::set_skip(
  const vector<TpgFault>& fault_list
)
{
  clear_skip_all();
  for ( auto f: fault_list ) {
    set_skip(f);
  }
}

// @brief 全ての故障のスキップマークを消す．
void
Fsim::clear_skip_all()
{
  mImpl->clear_skip_all();
}

// @brief 故障のスキップマークを消す．
void
Fsim::clear_skip(
  const TpgFault& f
)
{
  mImpl->clear_skip(f);
}

// @brief 複数の故障のスキップマークを消す．
void
Fsim::clear_skip(
  const TpgFaultList& fault_list
)
{
  set_skip_all();
  for ( auto f: fault_list ) {
    clear_skip(f);
  }
}

// @brief 複数の故障のスキップマークを消す．
void
Fsim::clear_skip(
  const vector<TpgFault>& fault_list
)
{
  set_skip_all();
  for ( auto f: fault_list ) {
    clear_skip(f);
  }
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const TestVector& tv,
  const TpgFault& f
)
{
  return mImpl->spsfp(tv, f);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const NodeValList& assign_list,
  const TpgFault& f
)
{
  return mImpl->spsfp(assign_list, f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
SizeType
Fsim::sppfp(
  const TestVector& tv
)
{
  return mImpl->sppfp(tv);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
SizeType
Fsim::sppfp(
  const NodeValList& assign_list
)
{
  return mImpl->sppfp(assign_list);
}

// @brief 複数のパタンで故障シミュレーションを行う．
SizeType
Fsim::ppsfp()
{
  return mImpl->ppsfp();
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
  return mImpl->calc_wsa(tv, weighted);
}

// @brief 状態を設定する．
void
Fsim::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
  mImpl->set_state(i_vect, f_vect);
}

// @brief 状態を取得する．
void
Fsim::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
  mImpl->get_state(i_vect, f_vect);
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  return mImpl->calc_wsa(i_vect, weighted);
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
Fsim::clear_patterns()
{
  mImpl->clear_patterns();
}

// @brief ppsfp 用のパタンを設定する．
void
Fsim::set_pattern(
  SizeType pos,
  const TestVector& tv
)
{
  mImpl->set_pattern(pos, tv);
}

// @brief 設定した ppsfp 用のパタンを読み出す．
TestVector
Fsim::get_pattern(
  SizeType pos
)
{
  return mImpl->get_pattern(pos);
}

// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
SizeType
Fsim::det_fault_num()
{
  return mImpl->det_fault_num();
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
TpgFault
Fsim::det_fault(
  SizeType pos
)
{
  return mImpl->det_fault(pos);
}

// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
vector<TpgFault>
Fsim::det_fault_list()
{
  return mImpl->det_fault_list();
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
PackedVal
Fsim::det_fault_pat(
  SizeType pos
)
{
  return mImpl->det_fault_pat(pos);
}

// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
const vector<PackedVal>&
Fsim::det_fault_pat_list()
{
  return mImpl->det_fault_pat_list();
}

END_NAMESPACE_DRUID
