#ifndef TD_FSIM_H
#define TD_FSIM_H

/// @file td/Fsim.h
/// @brief Fsim のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
/// @class Fsim Fsim.h "td/Fsim.h"
/// @brief 故障シミュレーションを行うクラスの基底クラス
///
/// 具体的には故障シミュレーションに特化した回路構造を表すクラスと
/// 故障シミュレーションに特化した故障の情報を表すクラスを持つ．
/// シミュレーションの際に検出された故障を以降のシミュレーションで
/// スキップするかどうかは外からコントロールされるべきなので，
/// このシミュレーションのみ有効な'スキップフラグ'というフラグを
/// 各故障に持たせる．スキップフラグは set_skip(f) で付加され，
/// clear_skip() で解除される．
//////////////////////////////////////////////////////////////////////
class Fsim
{
public:

  virtual
  ~Fsim() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  /// @param[in] network ネットワーク
  virtual
  void
  set_network(const TpgNetwork& network) = 0;

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all() = 0;

  /// @brief 故障にスキップマークをつける．
  /// @param[in] f 対象の故障
  virtual
  void
  set_skip(const TpgFault* f) = 0;

  /// @brief 複数の故障にスキップマークをつける．
  /// @param[in] fault_list 故障のリスト
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(const vector<const TpgFault*>& fault_list);

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all() = 0;

  /// @brief 故障のスキップマークを消す．
  /// @param[in] f 対象の故障
  virtual
  void
  clear_skip(const TpgFault* f) = 0;

  /// @brief 複数の故障のスキップマークを消す．
  /// @param[in] fault_list 故障のリスト
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(const vector<const TpgFault*>& fault_list);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(TestVector* tv,
	const TpgFault* f) = 0;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const NodeValList& assign_list,
	const TpgFault* f) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] op 検出した時に起動されるファンクタオブジェクト
  virtual
  void
  sppfp(TestVector* tv,
	FsimOp& op) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] op 検出した時に起動されるファンクタオブジェクト
  virtual
  void
  sppfp(const NodeValList& assign_list,
	FsimOp& op) = 0;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @param[in] tv_array テストベクタの配列
  /// @param[in] op 検出した時に起動されるファンクタオブジェクト
  virtual
  void
  ppsfp(const vector<TestVector*>& tv_array,
	FsimOp& op) = 0;

  /// @brief WSA を計算する．
  /// @param[in] tv テストベクタ
  virtual
  ymuint
  calc_wsa(TestVector* tv) = 0;

};


//////////////////////////////////////////////////////////////////////
// Fsim の派生クラスのインスタンスを生成する関数
//////////////////////////////////////////////////////////////////////

/// @brief 2値版の故障シミュレータ
extern
Fsim*
new_Fsim2();

/// @brief 3値版の故障シミュレータ
extern
Fsim*
new_Fsim3();

END_NAMESPACE_DRUID_TD

#endif // TD_FSIM_H
