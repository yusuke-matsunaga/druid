
/// @file Fsim2.cc
/// @brief Fsim2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Fsim2.h"
#include "Fsim2Impl.h"
#include "TpgFaultMgr.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

// 2値/3値，組み合わせ回路/ブロードサイド の4通りがある．
// すべて同じソースコードから生成するので名前空間だけが
// 異なっている．

namespace nsFsim2Combi2 {
  std::unique_ptr<Fsim2Impl>
  new_Fsim2(
    const TpgNetwork& network
  );
}

namespace nsFsim2Combi3 {
  std::unique_ptr<Fsim2Impl>
  new_Fsim2(
    const TpgNetwork& network
  );
}

namespace nsFsim2Bside2 {
  std::unique_ptr<Fsim2Impl>
  new_Fsim2(
    const TpgNetwork& network
  );
}

namespace nsFsim2Bside3 {
  std::unique_ptr<Fsim2Impl>
  new_Fsim2(
    const TpgNetwork& network
  );
}

BEGIN_NONAMESPACE

inline
std::unique_ptr<Fsim2Impl>
new_impl(
  const TpgNetwork& network,
  bool has_previous_state,
  bool has_x
)
{
  if ( has_x ) {
    // 3値バージョン
    if ( has_previous_state ) {
      return nsFsim2Bside3::new_Fsim2(network);
    }
    else {
      return nsFsim2Combi3::new_Fsim2(network);
    }
  }
  else {
    // 2値バージョン
    if ( has_previous_state ) {
      return nsFsim2Bside2::new_Fsim2(network);
    }
    else {
      return nsFsim2Combi2::new_Fsim2(network);
    }
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// Fsim2 の実装コード
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Fsim2::Fsim2()
{
}

// @brief デストラクタ
Fsim2::~Fsim2()
{
}

// @brief 初期化
void
Fsim2::initialize(
  const TpgNetwork& network,
  bool has_previous_state,
  bool has_x
)
{
  mImpl = new_impl(network, has_previous_state, has_x);
}

// @brief 対象の故障をセットする．
void
Fsim2::set_fault_list(
  const vector<TpgFault>& fault_list ///< [in] 故障のリスト
)
{
  mImpl->set_fault_list(fault_list);
}

// @brief 対象の故障をセットする．
void
Fsim2::set_fault_list(
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
Fsim2::set_skip_all()
{
  mImpl->set_skip_all();
}

// @brief 故障にスキップマークをつける．
void
Fsim2::set_skip(
  const TpgFault& f
)
{
  mImpl->set_skip(f);
}

// @brief 複数の故障にスキップマークをつける．
void
Fsim2::set_skip(
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
Fsim2::set_skip(
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
Fsim2::clear_skip_all()
{
  mImpl->clear_skip_all();
}

// @brief 故障のスキップマークを消す．
void
Fsim2::clear_skip(
  const TpgFault& f
)
{
  mImpl->clear_skip(f);
}

// @brief 複数の故障のスキップマークを消す．
void
Fsim2::clear_skip(
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
Fsim2::clear_skip(
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
Fsim2::get_skip(
  const TpgFault& f
) const
{
  return mImpl->get_skip(f);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim2::spsfp(
  const TestVector& tv,
  const TpgFault& f
)
{
  return mImpl->spsfp(tv, f);
}

// @brief SPSFP故障シミュレーションを行う．
bool
Fsim2::spsfp(
  const NodeValList& assign_list,
  const TpgFault& f
)
{
  return mImpl->spsfp(assign_list, f);
}

// @brief SPSFP故障シミュレーションを行う．
Fsim2::DiffVector
Fsim2::spsfp_diffvector()
{
  return mImpl->spsfp_diffvector();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
vector<TpgFault>
Fsim2::sppfp(
  const TestVector& tv
)
{
  return mImpl->sppfp(tv);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
vector<TpgFault>
Fsim2::sppfp(
  const NodeValList& assign_list
)
{
  return mImpl->sppfp(assign_list);
}

// @brief 直前の sppfp() に対する検出パタンのリストを返す．
Fsim2::DiffVector
Fsim2::sppfp_diffvector(
  TpgFault fault
)
{
  return mImpl->sppfp_diffvector(fault);
}

// @brief 複数のパタンで故障シミュレーションを行う．
bool
Fsim2::ppsfp(
  const vector<TestVector>& tv_list,
  cbtype callback
)
{
  return mImpl->ppsfp(tv_list, callback);
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim2::calc_wsa(
  const TestVector& tv,
  bool weighted
)
{
  return mImpl->calc_wsa(tv, weighted);
}

// @brief 状態を設定する．
void
Fsim2::set_state(
  const InputVector& i_vect,
  const DffVector& f_vect
)
{
  mImpl->set_state(i_vect, f_vect);
}

// @brief 状態を取得する．
void
Fsim2::get_state(
  InputVector& i_vect,
  DffVector& f_vect
)
{
  mImpl->get_state(i_vect, f_vect);
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
SizeType
Fsim2::calc_wsa(
  const InputVector& i_vect,
  bool weighted
)
{
  return mImpl->calc_wsa(i_vect, weighted);
}

END_NAMESPACE_DRUID
