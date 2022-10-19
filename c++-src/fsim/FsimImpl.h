#ifndef FSIMIMPL_H
#define FSIMIMPL_H

/// @file FsimImpl.h
/// @brief FsimImpl のヘッダファイル
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

//////////////////////////////////////////////////////////////////////
/// @class FsimImpl FsimImpl.h "FsimImpl.h"
/// @brief Fsim の実装クラス
//////////////////////////////////////////////////////////////////////
class FsimImpl
{
public:

  virtual
  ~FsimImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all() = 0;

  /// @brief 故障にスキップマークをつける．
  virtual
  void
  set_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) = 0;

  /// @brief 複数の故障にスキップマークをつける．
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(
    const vector<const TpgFault*>& fault_list ///< [in] 故障のリスト
  );

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all() = 0;

  /// @brief 故障のスキップマークを消す．
  virtual
  void
  clear_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) = 0;

  /// @brief 複数の故障のスキップマークを消す．
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(
    const vector<const TpgFault*>& fault_list ///< [in] 故障のリスト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    const TpgFault* f     ///< [in] 対象の故障
  ) = 0;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    const TpgFault* f               ///< [in] 対象の故障
  ) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  SizeType
  sppfp(
    const TestVector& tv ///< [in] テストベクタ
  ) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  SizeType
  sppfp(
    const NodeValList& assign_list ///< [in] 値の割当リスト
  ) = 0;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  SizeType
  ppsfp() = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 順序回路用のシミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  /// @param[in] tv
  ///
  /// weightedの意味は以下の通り
  virtual
  SizeType
  calc_wsa(
    const TestVector& tv, ///< [in] テストベクタ
                          /// - 外部入力以外は無視する．
                          /// - 時刻1の割り当ても無視する
    bool weighted         ///< [in] 重みフラグ
                          /// - false: ゲートの出力の遷移回数の和
                          /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)
                          ///          を掛けたものの和
  ) = 0;

  /// @brief 状態を設定する．
  virtual
  void
  set_state(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    const DffVector& f_vect    ///< [in] FFの値のビットベクタ
  ) = 0;

  /// @brief 状態を取得する．
  virtual
  void
  get_state(
    InputVector& i_vect, ///< [out] 外部入力のビットベクタ
    DffVector& f_vect    ///< [out] FFの値のビットベクタ
  ) = 0;

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  virtual
  SizeType
  calc_wsa(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    bool weighted              ///< [in] 重みフラグ
                               /// - false: ゲートの出力の遷移回数の和
                               /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)
                               ///          を掛けたものの和
  ) = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  virtual
  void
  clear_patterns() = 0;

  /// @brief ppsfp 用のパタンを設定する．
  virtual
  void
  set_pattern(
    SizeType pos,        ///< [in] 位置番号 ( 0 <= pos < PV_BITLEN )
    const TestVector& tv ///< [in] テストベクタ
  ) = 0;

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  virtual
  TestVector
  get_pattern(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < PV_BITLEN )
  ) = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  virtual
  SizeType
  det_fault_num() = 0;

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  virtual
  const TpgFault*
  det_fault(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  ) = 0;

  /// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
  virtual
  Array<const TpgFault*>
  det_fault_list() = 0;

  /// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
  virtual
  PackedVal
  det_fault_pat(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  ) = 0;

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
  virtual
  Array<PackedVal>
  det_fault_pat_list() = 0;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID

#endif // FSIMIMPL_H
