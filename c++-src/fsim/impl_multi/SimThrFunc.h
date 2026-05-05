#ifndef SIMTHRFUNC_H
#define SIMTHRFUNC_H

/// @file SimThrFunc.h
/// @brief SimThrFunc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "EventQ.h"
#include "DiffBitsArray.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimThrFunc SimThrFunc.h "SimThrFunc.h"
/// @brief スレッド実行の本体
///
/// オブジェクト自体はスレッドに対応して生成される．
/// 結果は FsimResultsRep に格納する．
//////////////////////////////////////////////////////////////////////
class SimThrFunc
{
public:

  /// @brief コンストラクタ
  SimThrFunc(
    SizeType id,                               ///< [in] ID番号
    FSIM_CLASSNAME& fsim,                      ///< [in] 故障シミュレータ本体
    const std::vector<const SimFFR*>& ffr_list ///< [in] 担当する FFR のリスト
    = {}
  );

  /// @brief デストラクタ
  ~SimThrFunc();


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

  /// @brief 担当する FFR のリストを返す．
  const std::vector<const SimFFR*>&
  ffr_list() const
  {
    return mFFRList;
  }

  /// @brief 正常値の計算を行う．
  void
  calc_gval(
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief 正常値の計算を行う．
  void
  calc_gval(
    const std::vector<TestVector>& tv ///< [in] テストベクタのリスト
  );

  /// @brief 正常値の計算を行う．
  void
  calc_gval(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief 正常値の計算を行う．
  void
  calc_gvalx(
    const AssignList& assign_list ///< [in] 入力割り当てのリスト
  );

  /// @brief SPSFP 法のシミュレーションを行う．
  bool
  spsfp(
    const SimFault* f ///< [in] 故障
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  ///
  /// 結果は mDetListArray[0] に格納される．
  void
  sppfp();

  /// @brief PPSFP 法のシミュレーションを行う．
  ///
  /// 結果は mDetListArray に格納される．
  void
  ppsfp();

  /// @brief SPSFP 法のシミュレーションを行う．
  DiffBits
  spsfp2(
    const SimFault* f ///< [in] 故障
  );

  /// @brief SPPFP 法のシミュレーションを行う．
  ///
  /// 結果は mDetListArray[0] と mDiffBitsListArray[0] に格納される．
  void
  sppfp2();

  /// @brief PPSFP 法のシミュレーションを行う．
  ///
  /// 結果は mDetListArray と mDiffBitsListArray に格納される．
  void
  ppsfp2();

  /// @brief SPPFP の結果を得る．
  const std::vector<SizeType>&
  det_list() const
  {
    return mDetListArray[0];
  }

  /// @brief PPSFP の結果を得る．
  const std::vector<SizeType>&
  det_list(
    SizeType tv_id ///< [in] テストベクタ番号
  ) const
  {
    return mDetListArray[tv_id];
  }

  /// @brief SPPFP2 の結果を得る．
  const std::vector<DiffBits>&
  diffbits_list() const
  {
    return mDiffBitsListArray[0];
  }

  /// @brief PPSFP2 の結果を得る．
  const std::vector<DiffBits>&
  diffbits_list(
    SizeType tv_id ///< [in] テストベクタ番号
  ) const
  {
    return mDiffBitsListArray[tv_id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp_sub(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
  );

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp2_sub(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
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
  PackedVal
  simulate();

  /// @brief イベントドリブンシミュレーションを行う．
  /// @retval 出力における変化ビットを返す．
  DiffBitsArray
  simulate2();

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

  // 故障シミュレータ
  FSIM_CLASSNAME& mFsim;

  // 担当する FFR のリスト
  std::vector<const SimFFR*> mFFRList;

  // 検出された故障のリストの配列
  std::vector<std::vector<SizeType>> mDetListArray;

  // 検出された故障の出力ごとの故障伝搬状況のリストの配列
  std::vector<std::vector<DiffBits>> mDiffBitsListArray;

  // 現在のテストベクタ数
  SizeType mTvNum;

  // イベントキュー
  EventQ mEventQ;

  // local_prop 用の値配列
  std::vector<FSIM_VALTYPE> mValArray;

#if FSIM_BSIDE
  // local_prop 用の値配列(1時刻前)
  std::vector<FSIM_VALTYPE> mPrevValArray;
#endif

  // clear 用の情報の配列
  std::vector<RestoreInfo> mClearArray;

  // ノード番号をキーにして反転マスクを保持する配列
  std::vector<PackedVal> mFlipMaskArray;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMTHRFUNC_H
