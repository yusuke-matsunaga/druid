#ifndef SIMENGINE_H
#define SIMENGINE_H

/// @file SimEngine.h
/// @brief SimEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "EventQ.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SyncObj;

//////////////////////////////////////////////////////////////////////
/// @class SimEngine SimEngine.h "SimEngine.h"
/// @brief シミュレーションを行う本体
//////////////////////////////////////////////////////////////////////
class SimEngine
{
  using cbtype1 = FsimImpl::cbtype1;
  using cbtype2 = FsimImpl::cbtype2;

public:

  /// @brief コンストラクタ
  SimEngine(
    SizeType id,                          ///< [in] ID番号
    SyncObj& simc_obj,                    ///< [in] 同期用オブジェクト
    FSIM_CLASSNAME& fsim,                 ///< [in] 故障シミュレータ本体
    const std::vector<const SimFFR*>& ffr_list ///< [in] 対象のFFRのリスト
  );

  /// @brief デストラクタ
  ~SimEngine();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief SPSFP 法のシミュレーションを行う．
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    const SimFault* f,    ///< [in] 故障
    DiffBits& dbits       ///< [out] 出力ごとの伝搬結果
  );

  /// @brief SPSFP 法のシミュレーションを行う．
  bool
  spsfp(
    const AssignList& assign_list, ///< [in] 入力割り当てのリスト
    const SimFault* f,             ///< [in] 故障
    DiffBits& dbits                ///< [out] 出力ごとの伝搬結果
  );

  /// @brief SPSFP 法のシミュレーションを行う．
  bool
  xspsfp(
    const AssignList& assign_list, ///< [in] 入力割り当てのリスト
    const SimFault* f,             ///< [in] 故障
    DiffBits& dbits                ///< [out] 出力ごとの伝搬結果
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp(
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  xsppfp(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief PPSFP 法のシミュレーションを行う．
  void
  ppsfp(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  );

  /// @brief SPPFP 法の結果に対してコールバック関数を呼び出す．
  void
  apply_callback1(
    cbtype1 callback ///< [in] コールバック関数
  );

  /// @brief PPSFP 法の結果に対してコールバック関数を呼び出す．
  void
  apply_callback2(
    cbtype2 callback ///< [in] コールバック関数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief spsfp() の下請け関数
  bool
  _spsfp(
    const SimFault* f,    ///< [in] 故障
    DiffBits& dbits       ///< [out] 出力ごとの伝搬結果
  );

  /// @brief sppf() の下請け関数
  void
  _sppfp();

  /// @brief _sppsp() 用の下請け関数
  void
  sppfp_simulation(
    const std::vector<const SimFFR*>& ffr_array ///< [in] イベントを挿入するFFRのリスト
  );

  /// @brief FFR内の個々の故障の故障伝搬条件を計算する．
  /// @return 全ての故障の伝搬結果のORを返す．
  ///
  /// 各 SimFault の obs_mask() に結果がセットされる．
  PackedVal
  foreach_faults(
    const SimFFR& ffr ///< [in] 対象のFFR
  );

  /// @brief FFR内の故障シミュレーションを行う．
  PackedVal
  local_prop(
    const SimFault* fault ///< [in] 対象の故障
  )
  {
    // 故障の活性化条件を求める．
    auto cval = fault->excitation_condition(mValArray);

    // FFR 内の故障伝搬を行う．
    auto lobs = PV_ALL1;
    auto f_node = fault->origin_node();
    for ( auto node = f_node; !node->is_ffr_root(); ) {
      auto onode = node->fanout_top();
      auto pos = node->fanout_ipos();
      lobs &= onode->_calc_gobs(mValArray, pos);
      node = onode;
    }

#if FSIM_BSIDE
    // 1時刻前の条件を求める．
    auto pval = fault->previous_condition(mPrevValArray);
    return cval & pval & lobs;
#else
    return cval & lobs;
#endif
  }

  void
  log(
    const std::string& msg
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 値を元に戻すための構造体
  struct RestoreInfo
  {
    // ノード番号
    SizeType mId;

    // 元の値
    FSIM_VALTYPE mVal;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // イベントシミュレーション用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの値を取り出す．
  FSIM_VALTYPE
  get_val(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    return mValArray[node->id()];
  }

  /// @brief ノードの値を設定する．
  void
  set_val(
    const SimNode* node, ///< [in] 対象のノード
    FSIM_VALTYPE val     ///< [in] 設定する値
  )
  {
    mValArray[node->id()] = val;
  }

  /// @brief 正常値の計算を行う．
  void
  _calc_gval(
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief 正常値の計算を行う．
  void
  _calc_gval(
    const std::vector<TestVector>& tv ///< [in] テストベクタのリスト
  );

  /// @brief 正常値の計算を行う．
  void
  _calc_gval(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief 正常値の計算を行う．
  void
  _xcalc_gval(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val(
    std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  )
  {
    for ( auto node: mFsim.logic_list() ) {
      auto val = node->calc_val(val_array);
      val_array[node->id()] = val;
    }
  }

  /// @brief 初期イベントを追加する．
  void
  put_event(
    const SimNode* node, ///< [in] 対象のノード
    PackedVal valmask    ///< [in] 反転マスク
  )
  {
    if ( node->gate_type() == PrimType::None ) {
      // 入力の場合，他のイベントの干渉は受けないので
      // 今計算してしまう．
      auto old_val = get_val(node);
      set_val(node, old_val ^ valmask);
      add_to_clear_list(node, old_val);
      mEventQ.put_fanouts(node);
    }
    else {
      // 複数のイベントを登録する場合があるので
      // ここでは計算せずに反転マスクのみをセットする．
      set_flip_mask(node, valmask);
      mEventQ.put(node);
    }
  }

  /// @brief イベントドリブンシミュレーションを行う．
  /// @retval 出力における変化ビットを返す．
  DiffBitsArray
  simulate();

  /// @brief clear リストに追加する．
  void
  add_to_clear_list(
    const SimNode* node, ///< [in] 対象のノード
    FSIM_VALTYPE old_val ///< [in] 元の値
  )
  {
    mClearArray.push_back({node->id(), old_val});
  }

  /// @brief 反転フラグをセットする．
  void
  set_flip_mask(
    const SimNode* node, ///< [in] 対象のノード
    PackedVal flip_mask  ///< [in] 反転マスク
  )
  {
    if ( mFlipMaskArray[node->id()] != PV_ALL0 ) {
      throw std::logic_error{"mFlipMaskArray[node->id()] != PV_ALL0"};
    }
    mFlipMaskArray[node->id()] = flip_mask;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 同期用オブジェクト
  SyncObj& mSyncObj;

  // 故障シミュレータ
  FSIM_CLASSNAME& mFsim;

  // 処理対象の FFR リスト
  std::vector<const SimFFR*> mFFRList;

  // ノード番号をキーにして反転マスクを保持する配列
  std::vector<PackedVal> mFlipMaskArray;

  // イベントキュー
  EventQ mEventQ;

  // local_prop 用の値配列
  std::vector<FSIM_VALTYPE> mValArray;

  // local_prop 用の値配列(1時刻前)
  std::vector<FSIM_VALTYPE> mPrevValArray;

  // clear 用の情報の配列
  std::vector<RestoreInfo> mClearArray;

  // 結果のリスト(SPPFP)
  std::vector<std::pair<SizeType, DiffBits>> mResList1;

  // 結果のリスト(PPSFP)
  std::vector<std::pair<SizeType, DiffBitsArray>> mResList2;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMENGINE_H
