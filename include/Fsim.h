#ifndef FSIM_H
#define FSIM_H

/// @file Fsim.h
/// @brief Fsim のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"
#include "PackedVal.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

class FsimImpl;

//////////////////////////////////////////////////////////////////////
/// @class Fsim Fsim.h "Fsim.h"
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
class Fsim
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  Fsim();

  /// @brief コピーコンストラクタは禁止
  Fsim(const Fsim& src) = delete;

  /// @brief ムーブコンストラクタはデフォルト
  Fsim(Fsim&& src) = default;

  /// @brief コピー代入演算子は禁止
  Fsim&
  operator=(const Fsim& src) = delete;

  /// @brief ムーブ代入演算子はデフォルト
  Fsim&
  operator=(Fsim&& src) = default;

  /// @brief デストラクタ
  ~Fsim();


public:
  //////////////////////////////////////////////////////////////////////
  // 初期化を行う．
  //////////////////////////////////////////////////////////////////////

  /// @brief 2値の故障シミュレータとして初期化する．
  void
  init_fsim2(
    const TpgNetwork& network, ///< [in] ネットワーク
    FaultType fault_type       ///< [in] 故障の型
  );

  /// @brief 3値の故障シミュレータとして初期化する．
  void
  init_fsim3(
    const TpgNetwork& network, ///< [in] ネットワーク
    FaultType fault_type       ///< [in] 故障の型
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    const TpgFault* f  ///< [in] 対象の故障
  );

  /// @brief 複数の故障にスキップマークをつける．
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(
    const vector<const TpgFault*>& fault_list  ///< [in] 故障のリスト

  );

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    const TpgFault* f  ///< [in] 対象の故障
  );

  /// @brief 複数の故障のスキップマークを消す．
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(
    const vector<const TpgFault*>& fault_list  ///< [in] 故障のリスト
  );


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
    const TpgFault* f	  ///< [in] 対象の故障
  );

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    const TpgFault* f		    ///< [in] 対象の故障
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  SizeType
  sppfp(
    const TestVector& tv  ///< [in] テストベクタ
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  SizeType
  sppfp(
    const NodeValList& assign_list  ///< [in] 値の割当リスト
  );

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  SizeType
  ppsfp();


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
    InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    DffVector& f_vect	 ///< [in] FFの値のビットベクタ
  );

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  SizeType
  calc_wsa(
    const InputVector& i_vect,  ///< [in] 外部入力のビットベクタ
    bool weighted               ///< [in] テストベクタ
				///< [in] 重みフラグ
				///< - false: ゲートの出力の遷移回数の和
				///< - true : ゲートの出力の遷移回数に
				///<          (ファンアウト数＋１)を掛けたものの和
  );


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  void
  clear_patterns();

  /// @brief ppsfp 用のパタンを設定する．
  void
  set_pattern(
    SizeType pos,        ///< [in] 位置番号 ( 0 <= pos < kPvBitLen )
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  TestVector
  get_pattern(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < kPvBitLen )
  );


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  SizeType
  det_fault_num();

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  const TpgFault*
  det_fault(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  );

  /// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
  Array<const TpgFault*>
  det_fault_list();

  /// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
  PackedVal
  det_fault_pat(
    SizeType pos  ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  );

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
  Array<PackedVal>
  det_fault_pat_list();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  std::unique_ptr<FsimImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // FSIM_H
