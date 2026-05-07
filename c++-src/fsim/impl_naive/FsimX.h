#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FsimImpl.h"
#include "fsim_nsdef.h"
#include "types/PackedVal.h"
#include "types/PackedVal3.h"
#include "SimEngine.h"
#include "SimFault.h"
#include "SimFFR.h"
#include "SimNodeList.h"
#include "types/TpgNode.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimNode;

BEGIN_NONAMESPACE

inline
std::string
val_str(
  FSIM_VALTYPE val,
  SizeType bitpos = 0
)
{
  PackedVal bit = 1UL << bitpos;
#if FSIM_VAL2
  if ( val & bit ) {
    return "1";
  }
  return "0";
#elif FSIM_VAL3
  if ( val.val0() & bit ) {
    return "0";
  }
  if ( val.val1() & bit ) {
    return "1";
  }
  return "X";
#endif
}


END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
/// @class FSIM_CLASSNAME FsimX.h "FsimX.h"
/// @brief 故障シミュレーションを行うモジュール
///
/// 実際のクラス名は FsimSa2, FsimSa3, FsimTd2, FsimTd3 である．
//////////////////////////////////////////////////////////////////////
class FSIM_CLASSNAME :
  public FsimImpl
{
public:

  /// @brief コンストラクタ
  FSIM_CLASSNAME (
    const TpgFaultList& fault_list ///< [in] 対象の故障のリスト
  );

  /// @brief デストラクタ
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all() override;

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    SizeType fid ///< [in] 対象の故障番号
  ) override;

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all() override;

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    SizeType fid ///< [in] 対象の故障番号
  ) override;

  /// @brief 故障のスキップマークを得る．
  bool
  get_skip(
    SizeType fid ///< [in] 対象の故障番号
  ) const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 検出の判定のみを行う故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    SizeType fid          ///< [in] 対象の故障番号
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    SizeType fid                   ///< [in] 対象の故障番号
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  bool
  xspsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    SizeType fid                   ///< [in] 対象の故障番号
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障番号のリストを返す．
  std::vector<SizeType>
  sppfp(
    const TestVector& tv ///< [in] テストベクタ
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障番号のリストを返す．
  std::vector<SizeType>
  sppfp(
    const AssignList& assign_list ///< [in] 値の割当リスト
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障番号のリストを返す．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  std::vector<SizeType>
  xsppfp(
    const AssignList& assign_list ///< [in] 値の割当リスト
  ) override;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  std::vector<std::vector<SizeType>>
  ppsfp(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // 出力ごとの故障伝搬を調べる故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  DiffBits
  spsfp2(
    const TestVector& tv, ///< [in] テストベクタ
    SizeType fid          ///< [in] 対象の故障番号
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  DiffBits
  spsfp2(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    SizeType fid                   ///< [in] 対象の故障番号
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  DiffBits
  xspsfp2(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    SizeType fid                   ///< [in] 対象の故障番号
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  FsimResultsRep*
  sppfp2(
    const TestVector& tv ///< [in] テストベクタ
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  FsimResultsRep*
  sppfp2(
    const AssignList& assign_list ///< [in] 値の割当リスト
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  FsimResultsRep*
  xsppfp2(
    const AssignList& assign_list ///< [in] 値の割当リスト
  ) override;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  std::vector<FsimResultsRep*>
  ppsfp2(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // 順序回路用のシミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  /// @param[in] tv テストベクタ
  ///
  /// - 外部入力以外は無視する．
  /// - 時刻1の割り当ても無視する
  /// weightedの意味は以下の通り
  /// - false: ゲートの出力の遷移回数の和
  /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
  SizeType
  calc_wsa(
    const TestVector& tv, ///< [in] テストベクタ
                          /// - 外部入力以外は無視する．
                          /// - 時刻1の割り当ても無視する
    bool weighted         ///< [in] 重みフラグ
                          /// - false: ゲートの出力の遷移回数の和
                          /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)
                          ///          を掛けたものの和
  ) override;

  /// @brief 状態を設定する．
  void
  set_state(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    const DffVector& f_vect    ///< [in] FFの値のビットベクタ
  ) override;

  /// @brief 状態を取得する．
  void
  get_state(
    InputVector& i_vect, ///< [out] 外部入力のビットベクタ
    DffVector& f_vect    ///< [out] FFの値のビットベクタ
  ) override;

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  SizeType
  calc_wsa(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    bool weighted              ///< [in] 重みフラグ
                               /// - false: ゲートの出力の遷移回数の和
                               /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)
                               ///          を掛けたものの和
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // 内部のデータ構造にアクセスする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を返す．
  SizeType
  input_num() const
  {
    return mEngine.input_num();
  }

  /// @brief PPI数を返す．
  SizeType
  ppi_num() const
  {
    return mEngine.ppi_num();
  }

  /// @brief PPI のノードを返す．
  SimNode*
  ppi(
    SizeType id ///< [in] PPI番号 ( 0 <= id < ppi_num() )
  ) const
  {
    return mEngine.ppi(id);
  }

  /// @brief 外部入力ノードのリストを返す．
  SimNodeList
  input_list() const
  {
    return mEngine.input_list();
  }

  /// @brief DFFの出力ノードのリストを返す．
  SimNodeList
  dff_output_list() const
  {
    return mEngine.dff_output_list();
  }

  /// @brief PPI のノードのリストを返す．
  SimNodeList
  ppi_list() const
  {
    return mEngine.ppi_list();
  }

  /// @brief PPO数を返す．
  SizeType
  ppo_num() const
  {
    return mEngine.ppo_num();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションの本体
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief SPPFP故障シミュレーションの本体
  void
  _sppfp();

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp_simulation(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
  );

  /// @brief PPSFP故障シミュレーションの本体
  void
  _ppsfp(
    SizeType base, ///< [in] パタン番号の起点
    SizeType npat  ///< [in] パタン数
  );

  /// @brief SPSFP故障シミュレーションの本体
  /// @return 出力ごとの故障伝搬状況
  DiffBits
  _spsfp2(
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief SPPFP故障シミュレーションの本体
  void
  _sppfp2();

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp2_simulation(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
  );

  /// @brief PPSFP故障シミュレーションの本体
  void
  _ppsfp2(
    SizeType base, ///< [in] パタン番号の起点
    SizeType npat  ///< [in] パタン数
  );

#if 0
  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @return 伝搬したビットに1を立てたビットベクタ
  ///
  /// obs_mask が0のビットのイベントはマスクされる．
  PackedVal
  _global_prop(
    SimNode* root,     ///< [in] FFRの根のノード
    PackedVal obs_mask ///< [in] ビットマスク
  )
  {
    mEngine.put_event(root, obs_mask);
    return mEngine.simulate();
  }

  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @return 伝搬したビットに1を立てたビットベクタ
  ///
  /// obs_mask が0のビットのイベントはマスクされる．
  DiffBitsArray
  _global_prop2(
    SimNode* root,     ///< [in] FFRの根のノード
    PackedVal obs_mask ///< [in] ビットマスク
  )
  {
    mEngine.put_event(root, obs_mask);
    return mEngine.simulate2();
  }

  /// @brief FFR内の故障シミュレーションを行う．
  PackedVal
  _local_prop(
    SimFault* fault ///< [in] 対象の故障
  )
  {
    // 故障の活性化条件を求める．
    auto cval = fault->excitation_condition();
    // FFR 内の故障伝搬を行う．
    auto lobs = PV_ALL1;
    auto f_node = fault->origin_node();
    for ( auto node = f_node; !node->is_ffr_root(); ) {
      auto onode = node->fanout_top();
      auto pos = node->fanout_ipos();
      lobs &= onode->_calc_gobs(pos);
      node = onode;
    }

#if FSIM_BSIDE
    // 1時刻前の条件を求める．
    auto pval = fault->previous_condition();
    return cval & pval & lobs;
#else
    return cval & lobs;
#endif
  }

  /// @brief sppfp 用の下請け関数
  void
  _sppfp_sub(
    const SimFFR& ffr ///< [in] 対象の FFR
  )
  {
    auto& fault_list = ffr.fault_list();
    for ( auto ff: fault_list ) {
      if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	auto fid = ff->id();
	mDetList.push_back(fid);
      }
    }
  }

  /// @brief sppfp 用の下請け関数
  void
  _sppfp2_sub(
    const SimFFR& ffr, ///< [in] 対象の FFR
    DiffBits dbits     ///< [in] 出力の故障伝搬ビット
  )
  {
    auto& fault_list = ffr.fault_list();
    for ( auto ff: fault_list ) {
      if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	auto fid = ff->id();
	mDetList.push_back(fid);
	mDiffBitsList.push_back(dbits);
      }
    }
  }

  /// @brief FFR 内の個々の故障の故障伝搬条件を計算する．
  /// @return 全ての故障の伝搬結果のORを返す．
  ///
  /// 個々の SimFault の obs_mask を設定する．
  PackedVal
  _foreach_faults(
    const SimFFR& ffr ///< [in] 対象の FFR
  )
  {
    auto& fault_list = ffr.fault_list();
    auto ffr_req = PV_ALL0;
    for ( auto ff: fault_list ) {
      if ( ff->skip() ) {
	continue;
      }
      auto obs = _local_prop(ff);
      ff->set_obs_mask(obs);
      ffr_req |= obs;
    }

    return ffr_req;
  }
#endif


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // エンジン
  SimEngine mEngine;

  // 検出された故障番号のリスト
  std::vector<SizeType> mDetList;

  // 検出された故障の出力ごとの故障伝搬状況のリスト
  std::vector<DiffBits> mDiffBitsList;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIMX_H
