﻿
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Fsim.h"
#include "FsimImpl.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

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
  bool has_previous_state,
  bool has_x
)
{
  initialize_naive(network, has_previous_state, has_x);
}

// @brief 対象の故障をセットする．
void
Fsim::set_fault_list(
  const vector<TpgFault>& fault_list ///< [in] 故障のリスト
)
{
  mImpl->set_fault_list(fault_list);
}

// @brief 対象の故障をセットする．
void
Fsim::set_fault_list(
  const TpgFaultList& fault_list ///< [in] 故障のリスト
)
{
  vector<TpgFault> tmp_list;
  tmp_list.reserve(fault_list.size());
  for ( auto f: fault_list ) {
    tmp_list.push_back(f);
  }
  mImpl->set_fault_list(tmp_list);
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

// @brief 故障のスキップマークを得る．
bool
Fsim::get_skip(
  const TpgFault& f
) const
{
  return mImpl->get_skip(f);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const TestVector& tv,
  const TpgFault& f,
  DiffBits& dbits
)
{
  return mImpl->spsfp(tv, f, dbits);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim::spsfp(
  const NodeValList& assign_list,
  const TpgFault& f,
  DiffBits& dbits
)
{
  return mImpl->spsfp(assign_list, f, dbits);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
Fsim::sppfp(
  const TestVector& tv,
  cbtype callback
)
{
  return mImpl->sppfp(tv, callback);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
void
Fsim::sppfp(
  const NodeValList& assign_list,
  cbtype callback
)
{
  return mImpl->sppfp(assign_list, callback);
}

// @brief 複数のパタンで故障シミュレーションを行う．
void
Fsim::ppsfp(
  const vector<TestVector>& tv_list,
  cbtype callback
)
{
  return mImpl->ppsfp(tv_list, callback);
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

END_NAMESPACE_DRUID
