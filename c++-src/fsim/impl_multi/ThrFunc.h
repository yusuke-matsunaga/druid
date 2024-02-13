#ifndef THRFUNC_H
#define THRFUNC_H

/// @file ThrFunc.h
/// @brief ThrFunc のヘッダファイル
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
/// @class ThrFunc ThrFunc.h "ThrFunc.h"
/// @brief ppsfp タイプのイベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class ThrFunc
{
public:

  /// @brief コンストラクタ
  ThrFunc(
    SizeType id,          ///< [in] ID番号
    FSIM_CLASSNAME& fsim, ///< [in] 故障シミュレータ本体
    SyncObj& sync         ///< [in] 同期用のオブジェクト
  );

  /// @brief デストラクタ
  ~ThrFunc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief スレッド実行の本体
  void
  main_loop();

  /// @brief 結果を読み出す．
  const vector<pair<TpgFault, DiffBits>>&
  res_list(
    SizeType bit ///< [in] ビット位置
  ) const
  {
    return mResList[bit];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief PPSFP 法のシミュレーションを行う．
  void
  ppsfp();

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp();

  /// @brief sppsp() 用の下請け関数
  void
  sppfp_simulation(
    const vector<const SimFFR*>& ffr_array ///< [in] 対象のFFRのリスト
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
    SimFault* fault ///< [in] 対象の故障
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
    const string& msg
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
    const InputVals& input_vals      ///< [in] 入力値
  );

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val(
    vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
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
  ///
  /// 返されるベクタのサイズは output_num + 1
  /// 最後の要素は全ての出力のORになっている．
  vector<PackedVal>
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
    ASSERT_COND( mFlipMaskArray[node->id()] == PV_ALL0 );
    mFlipMaskArray[node->id()] = flip_mask;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 故障シミュレータ
  FSIM_CLASSNAME& mFsim;

  // 同期用のオブジェクト
  SyncObj& mSyncObj;

  // 出力ごとの故障伝搬パタンの配列
  // サイズは output_num + 1
  vector<PackedVal> mPropArray;

  // ノード番号をキーにして反転マスクを保持する配列
  vector<PackedVal> mFlipMaskArray;

  // イベントキュー
  EventQ mEventQ;

  // local_prop 用の値配列
  vector<FSIM_VALTYPE> mValArray;

  // local_prop 用の値配列(1時刻前)
  vector<FSIM_VALTYPE> mPrevValArray;

  // clear 用の情報の配列
  vector<RestoreInfo> mClearArray;

  // 一時的に結果を貯めておくバッファの排列
  vector<pair<TpgFault, DiffBits>> mResList[PV_BITLEN];

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID_FSIM

#endif // THRFUNC_H
