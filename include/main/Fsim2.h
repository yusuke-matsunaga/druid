#ifndef FSIM2_H
#define FSIM2_H

/// @file Fsim2.h
/// @brief Fsim2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"
#include "TpgFaultList.h"
#include "PackedVal.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

class Fsim2Impl;

//////////////////////////////////////////////////////////////////////
/// @class Fsim2 Fsim2.h "Fsim2.h"
/// @brief 故障シミュレーションを行うクラスの基底クラス
///
/// 具体的には故障シミュレーションに特化した回路構造を表すクラスと
/// 故障シミュレーションに特化した故障の情報を表すクラスを持つ．
/// シミュレーションの際に検出された故障を以降のシミュレーションで
/// スキップするかどうかは外からコントロールされるべきなので，
/// このシミュレーションのみ有効な'スキップフラグ'というフラグを
/// 各故障に持たせる．スキップフラグは set_skip(f) で付加され，
/// clear_skip(f) で解除される．
//////////////////////////////////////////////////////////////////////
class Fsim2
{
public:

  /// @brief 出力ごとの故障伝搬状況を表すビットベクタ
  using DiffVector = vector<bool>;

public:

  /// @brief コンストラクタ
  Fsim2();

  /// @brief コピーコンストラクタは禁止
  Fsim2(const Fsim2& src) = delete;

  /// @brief ムーブコンストラクタはデフォルト
  Fsim2(Fsim2&& src) = default;

  /// @brief コピー代入演算子は禁止
  Fsim2&
  operator=(const Fsim2& src) = delete;

  /// @brief ムーブ代入演算子はデフォルト
  Fsim2&
  operator=(Fsim2&& src) = default;

  /// @brief デストラクタ
  ~Fsim2();


public:
  //////////////////////////////////////////////////////////////////////
  // 初期化
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  initialize(
    const TpgNetwork& network, ///< [in] ネットワーク
    bool has_previous_state,   ///< [in] 1時刻前の値を持つ時 true にする．
    bool has_x                 ///< [in] 3値のシミュレーションを行う時 true にする．
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障をセットする．
  void
  set_fault_list(
    const vector<TpgFault>& fault_list ///< [in] 故障のリスト
  );

  /// @brief 対象の故障をセットする．
  void
  set_fault_list(
    const TpgFaultList& fault_list ///< [in] 故障のリスト
  );

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    const TpgFault& f  ///< [in] 対象の故障
  );

  /// @brief 複数の故障にスキップマークをつける．
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(
    const TpgFaultList& fault_list  ///< [in] 故障のリスト

  );

  /// @brief 複数の故障にスキップマークをつける．
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(
    const vector<TpgFault>& fault_list  ///< [in] 故障のリスト
  );

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    const TpgFault& f  ///< [in] 対象の故障
  );

  /// @brief 複数の故障のスキップマークを消す．
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(
    const TpgFaultList& fault_list  ///< [in] 故障のリスト
  );

  /// @brief 複数の故障のスキップマークを消す．
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(
    const vector<TpgFault>& fault_list  ///< [in] 故障のリスト
  );

  /// @brief 故障のスキップマークを得る．
  bool
  get_skip(
    const TpgFault& f ///< [in] 対象の故障
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    const TpgFault& f	  ///< [in] 対象の故障
  );

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    const TpgFault& f		    ///< [in] 対象の故障
  );

  /// @brief 直前の spsfp() に対する検出パタンを返す．
  DiffVector
  spsfp_diffvector();

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障のリストを返す．
  vector<TpgFault>
  sppfp(
    const TestVector& tv  ///< [in] テストベクタ
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障のリストを返す．
  vector<TpgFault>
  sppfp(
    const NodeValList& assign_list  ///< [in] 値の割当リスト
  );

  /// @brief 直前の sppfp() に対する検出パタンのリストを返す．
  vector<DiffVector>
  sppfp_diffvector();

  /// @brief ppsfp で用いるコールバック関数の型定義
  ///
  /// * 1番目の引数はパタン番号(tv_list中の位置)
  /// * 2番目の引数はテストパタン
  /// * 3番目の引数は検出された故障
  /// * false が返された時には処理を中断する．
  using cbtype = std::function<bool(SizeType, TestVector, TpgFault)>;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 全パタンシミュレーションした場合に true を返す．
  ///
  /// callback 関数が false を返した場合にはこの関数も false を返す．
  bool
  ppsfp(
    const vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    cbtype callback                    ///< [in] コールバック関数
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 順序回路用のシミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  ///
  /// - 外部入力以外は無視する．
  /// - 時刻1の割り当ても無視する
  SizeType
  calc_wsa(
    const TestVector& tv,  ///< [in] テストベクタ
    bool weighted          ///< [in] 重みフラグ
                           ///< - false: ゲートの出力の遷移回数の和
                           ///< - true : ゲートの出力の遷移回数に
                           ///<          (ファンアウト数＋１)を掛けたものの和
  );

  /// @brief 状態を設定する．
  void
  set_state(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    const DffVector& f_vect    ///< [in] FFの値のビットベクタ
  );

  /// @brief 状態を取得する．
  void
  get_state(
    InputVector& i_vect, ///< [out] 外部入力のビットベクタ
    DffVector& f_vect	 ///< [out] FFの値のビットベクタ
  );

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  SizeType
  calc_wsa(
    const InputVector& i_vect,  ///< [in] 外部入力のビットベクタ
    bool weighted               ///< [in] 重みフラグ
				///< - false: ゲートの出力の遷移回数の和
				///< - true : ゲートの出力の遷移回数に
				///<          (ファンアウト数＋１)を掛けたものの和
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  std::unique_ptr<Fsim2Impl> mImpl;

};

END_NAMESPACE_DRUID

#endif // FSIM2_H
