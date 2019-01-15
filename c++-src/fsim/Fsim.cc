
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim.h"
#include "FsimImpl.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

namespace nsFsimSa2 {
  std::unique_ptr<FsimImpl> new_Fsim(const TpgNetwork& network);
}

namespace nsFsimSa3 {
  std::unique_ptr<FsimImpl> new_Fsim(const TpgNetwork& network);
}

namespace nsFsimTd2 {
  std::unique_ptr<FsimImpl> new_Fsim(const TpgNetwork& network);
}

namespace nsFsimTd3 {
  std::unique_ptr<FsimImpl> new_Fsim(const TpgNetwork& network);
}


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
//
// 内容は不定
Fsim::Fsim()
{
}

// @brief デストラクタ
Fsim::~Fsim()
{
}

// @brief 2値の故障シミュレータとして初期化する．
// @param[in] network ネットワーク
// @param[in] fault_type 故障の型
void
Fsim::init_fsim2(const TpgNetwork& network,
		 FaultType fault_type)
{
  if ( fault_type == FaultType::StuckAt ) {
    mImpl = nsFsimSa2::new_Fsim(network);
  }
  else if ( fault_type == FaultType::TransitionDelay ) {
    mImpl = nsFsimTd2::new_Fsim(network);
  }
  else {
    ASSERT_NOT_REACHED;
  }
}

// @brief 3値の故障シミュレータとして初期化する．
// @param[in] network ネットワーク
// @param[in] fault_type 故障の型
void
Fsim::init_fsim3(const TpgNetwork& network,
		 FaultType fault_type)
{
  if ( fault_type == FaultType::StuckAt ) {
    mImpl = nsFsimSa3::new_Fsim(network);
  }
  else if ( fault_type == FaultType::TransitionDelay ) {
    mImpl = nsFsimTd3::new_Fsim(network);
  }
  else {
    ASSERT_NOT_REACHED;
  }
}

// @brief 全ての故障にスキップマークをつける．
void
Fsim::set_skip_all()
{
  if ( mImpl ) {
    mImpl->set_skip_all();
  }
}

// @brief 故障にスキップマークをつける．
// @param[in] f 対象の故障
void
Fsim::set_skip(const TpgFault* f)
{
  if ( mImpl ) {
    mImpl->set_skip(f);
  }
}

// @brief 複数の故障にスキップマークをつける．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは消される．
void
Fsim::set_skip(const vector<const TpgFault*>& fault_list)
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
  if ( mImpl ) {
    mImpl->clear_skip_all();
  }
}

// @brief 故障のスキップマークを消す．
// @param[in] f 対象の故障
void
Fsim::clear_skip(const TpgFault* f)
{
  if ( mImpl ) {
    mImpl->clear_skip(f);
  }
}

// @brief 複数の故障のスキップマークを消す．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは付けられる．
void
Fsim::clear_skip(const vector<const TpgFault*>& fault_list)
{
  set_skip_all();
  for ( auto f: fault_list ) {
    clear_skip(f);
  }
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim::spsfp(const TestVector& tv,
	    const TpgFault* f)
{
  if ( mImpl ) {
    return mImpl->spsfp(tv, f);
  }
  else {
    return false;
  }
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim::spsfp(const NodeValList& assign_list,
	    const TpgFault* f)
{
  if ( mImpl ) {
    return mImpl->spsfp(assign_list, f);
  }
  else {
    return false;
  }
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
Fsim::sppfp(const TestVector& tv)
{
  if ( mImpl ) {
    return mImpl->sppfp(tv);
  }
  else {
    return 0;
  }
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
Fsim::sppfp(const NodeValList& assign_list)
{
  if ( mImpl ) {
    return mImpl->sppfp(assign_list);
  }
  else {
    return 0;
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
int
Fsim::ppsfp()
{
  if ( mImpl ) {
    return mImpl->ppsfp();
  }
  else {
    return 0;
  }
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
// @param[in] tv テストベクタ
//
// - 外部入力以外は無視する．
// - 時刻1の割り当ても無視する
// weightedの意味は以下の通り
// - false: ゲートの出力の遷移回数の和
// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
int
Fsim::calc_wsa(const TestVector& tv,
	       bool weighted)
{
  if ( mImpl ) {
    return mImpl->calc_wsa(tv, weighted);
  }
  else {
    return 0;
  }
}

// @brief 状態を設定する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
Fsim::set_state(const InputVector& i_vect,
		const DffVector& f_vect)
{
  if ( mImpl ) {
    mImpl->set_state(i_vect, f_vect);
  }
}

// @brief 状態を取得する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
Fsim::get_state(InputVector& i_vect,
		DffVector& f_vect)
{
  if ( mImpl ) {
    mImpl->get_state(i_vect, f_vect);
  }
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
// @param[in] i_vect 外部入力のビットベクタ
int
Fsim::calc_wsa(const InputVector& i_vect,
	       bool weighted)
{
  if ( mImpl ) {
    return mImpl->calc_wsa(i_vect, weighted);
  }
  else {
    return 0;
  }
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
Fsim::clear_patterns()
{
  if ( mImpl ) {
    mImpl->clear_patterns();
  }
}

// @brief ppsfp 用のパタンを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
// @param[in] tv テストベクタ
void
Fsim::set_pattern(int pos,
		  const TestVector& tv)
{
  if ( mImpl ) {
    mImpl->set_pattern(pos, tv);
  }
}

// @brief 設定した ppsfp 用のパタンを読み出す．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
TestVector
Fsim::get_pattern(int pos)
{
  if ( mImpl ) {
    return mImpl->get_pattern(pos);
  }
  else {
    return TestVector();
  }
}

// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
int
Fsim::det_fault_num()
{
  if ( mImpl ) {
    return mImpl->det_fault_num();
  }
  else {
    return 0;
  }
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
const TpgFault*
Fsim::det_fault(int pos)
{
  if ( mImpl ) {
    return mImpl->det_fault(pos);
  }
  else {
    return nullptr;
  }
}

// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
Array<const TpgFault*>
Fsim::det_fault_list()
{
  if ( mImpl ) {
    return mImpl->det_fault_list();
  }
  else {
    return Array<const TpgFault*>(nullptr, 0, 0);
  }
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
PackedVal
Fsim::det_fault_pat(int pos)
{
  if ( mImpl ) {
    return mImpl->det_fault_pat(pos);
  }
  else {
    return 0UL;
  }
}

// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
Array<PackedVal>
Fsim::det_fault_pat_list()
{
  if ( mImpl ) {
    return mImpl->det_fault_pat_list();
  }
  else {
    return Array<PackedVal>(nullptr, 0, 0);
  }
}

END_NAMESPACE_DRUID
