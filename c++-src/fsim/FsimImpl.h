﻿#ifndef FSIMIMPL_H
#define FSIMIMPL_H

/// @file FsimImpl.h
/// @brief FsimImpl のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "FaultType.h"
#include "PackedVal.h"
#include "DiffBits.h"


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

  /// @brief 故障のスキップマークを得る．
  virtual
  bool
  get_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief sppfp で用いるコールバック関数の型定義
  using cbtype1 = Fsim::cbtype1;

  /// @brief ppsfp で用いるコールバック関数の型定義
  using cbtype2 = Fsim::cbtype2;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    const TpgFault* f,    ///< [in] 対象の故障
    DiffBits& dbits       ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) = 0;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    const TpgFault* f,                  ///< [in] 対象の故障
    DiffBits& dbits                     ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) = 0;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  virtual
  bool
  xspsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    const TpgFault* f,             ///< [in] 対象の故障
    DiffBits& dbits                ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  virtual
  void
  sppfp(
    const TestVector& tv, ///< [in] テストベクタ
    cbtype1 callback      ///< [in] コールバック関数
                          ///<      1番目の引数は検出された故障
                          ///<      2番目の引数は出力の伝搬状況
  ) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  virtual
  void
  sppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback                    ///< [in] コールバック関数
                                        ///<      1番目の引数は検出された故障
                                        ///<      2番目の引数は出力の伝搬状況
  ) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  virtual
  void
  xsppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback               ///< [in] コールバック関数
                                   ///<      1番目の引数は検出された故障
                                   ///<      2番目の引数は出力の伝搬状況
  ) = 0;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  virtual
  void
  ppsfp(
    const vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    cbtype2 callback                   ///< [in] 1回のシミュレーションごとに
                                       ///<      呼び出される関数
                                       ///<      1番目の引数は検出された故障
                                       ///<      2番目の引数は出力ごとの伝搬状況
  ) = 0;


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

};

END_NAMESPACE_DRUID

#endif // FSIMIMPL_H
