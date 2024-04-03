#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FsimImpl.h"
#include "fsim_nsdef.h"
#include "PackedVal.h"
#include "PackedVal3.h"
#include "EventQ.h"
#include "SimFault.h"
#include "SimFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "SimNodeList.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimNode;
class InputVals;

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
    const TpgNetwork& network ///< [in] ネットワーク
  );

  /// @brief デストラクタ
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障をセットする．
  void
  set_fault_list(
    const vector<const TpgFault*>& fault_list ///< [in] 故障のリスト
  ) override;

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all() override;

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) override;

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all() override;

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) override;

  /// @brief 故障のスキップマークを得る．
  bool
  get_skip(
    const TpgFault* f ///< [in] 対象の故障
  ) const override;


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
    const TpgFault* f,    ///< [in] 対象の故障
    DiffBits& dbits       ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    const TpgFault* f,              ///< [in] 対象の故障
    DiffBits& dbits                 ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  void
  sppfp(
    const TestVector& tv, ///< [in] テストベクタ
    cbtype1 callback      ///< [in] コールバック関数
                          ///<      1番目の引数は検出された故障
                          ///<      2番目の引数は出力の伝搬状況
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  void
  sppfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback                ///< [in] コールバック関数
                                    ///<      1番目の引数は検出された故障
                                    ///<      2番目の引数は出力の伝搬状況
  ) override;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  void
  ppsfp(
    const vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    cbtype2 callback                   ///< [in] 1回のシミュレーションごとに
                                       ///<      呼び出される関数
                                       ///<      1番目の引数は検出された故障
                                       ///<      2番目の引数は出力の伝搬状況
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
  // InputVals が用いる．
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を返す．
  SizeType
  input_num() const
  {
    return mInputNum;
  }

  /// @brief PPI数を返す．
  SizeType
  ppi_num() const
  {
    return mInputNum + mDffNum;
  }

  /// @brief PPI のノードを返す．
  SimNode*
  ppi(
    SizeType id ///< [in] PPI番号 ( 0 <= id < ppi_num() )
  ) const
  {
    ASSERT_COND( id >= 0 && id < ppi_num() );

    return mPPIList[id];
  }

  /// @brief 外部入力ノードのリストを返す．
  SimNodeList
  input_list() const
  {
    return SimNodeList{mPPIList.begin(), mPPIList.begin() + input_num()};
  }

  /// @brief DFFの出力ノードのリストを返す．
  SimNodeList
  dff_output_list() const
  {
    return SimNodeList{mPPIList.begin() + input_num(), mPPIList.end()};
  }

  /// @brief PPI のノードのリストを返す．
  SimNodeList
  ppi_list() const
  {
    return SimNodeList{mPPIList.begin(), mPPIList.end()};
  }

  /// @brief PPO数を返す．
  SizeType
  ppo_num() const
  {
    return mOutputNum + mDffNum;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  ///
  /// 全ての故障のスキップマークはクリアされる．
  void
  set_network(
    const TpgNetwork& network ///< [in] ネットワーク
  );

  /// @brief SPSFP故障シミュレーションの本体
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(
    const InputVals& iv, ///< [in] 入力値
    const TpgFault* f,   ///< [in] 対象の故障
    DiffBits& dbits      ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  );

  /// @brief SPPFP故障シミュレーションの本体
  void
  _sppfp(
    const InputVals& iv, ///< [in] 入力値
    cbtype1 callback     ///< [in] コールバック関数
  );

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp_simulation(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num,         ///< [in] FFR 数
    cbtype1 callback          ///< [in] コールバック関数
  );

  /// @brief PPSFP故障シミュレーションの本体
  /// @return callback() が false を返したら false を返す．
  bool
  _ppsfp(
    SizeType base,   ///< [in] パタン番号の起点
    SizeType npat,   ///< [in] パタン数
    cbtype2 callback ///< [in] コールバック関数
  );

  /// @brief 正常値の計算を行う．
  void
  _calc_gval(
    const InputVals& input_vals ///< [in] 入力値
  );

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val()
  {
    for ( auto node: mLogicArray ) {
      node->calc_val();
    }
  }

  /// @brief ノードの出力の(重み付き)信号遷移回数を求める．
  SizeType
  _calc_wsa(
    SimNode* node,
    bool weighted
  );

  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @return 伝搬したビットに1を立てたビットベクタ
  ///
  /// obs_mask が0のビットのイベントはマスクされる．
  DiffBitsArray
  _global_prop(
    SimNode* root,     ///< [in] FFRの根のノード
    PackedVal obs_mask ///< [in] ビットマスク
  )
  {
    mEventQ.put_event(root, obs_mask);
    return mEventQ.simulate();
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
  _sppfp_apply_callback(
    const SimFFR& ffr, ///< [in] 対象の FFR
    DiffBits dbits,    ///< [in] 出力の故障伝搬ビット
    cbtype1 callback   ///< [in] コールバック関数
  )
  {
    auto& fault_list = ffr.fault_list();
    for ( auto ff: fault_list ) {
      if ( !ff->skip() && ff->obs_mask() != PV_ALL0 ) {
	callback(ff->tpg_fault(), dbits);
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


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

  /// @brief 外部出力ノードを作る．
  SimNode*
  make_output(
    SimNode* input,
    SizeType output_id
  )
  {
    // 実際にはバッファタイプのノードに出力の印をつけるだけ．
    auto node = make_gate(PrimType::Buff, {input});
    node->set_output(output_id);
    return node;
  }

  /// @brief logic ノードを作る．
  SimNode*
  make_gate(
    PrimType type,
    const vector<SimNode*>& inputs
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部入力数
  SizeType mInputNum;

  // 外部出力数
  SizeType mOutputNum;

  // DFF数
  SizeType mDffNum;

  // 全ての SimNode を納めた配列
  vector<unique_ptr<SimNode>> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  vector<SimNode*> mPPIList;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  vector<SimNode*> mPPOList;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // TpgNode のノード番号から SimNode を取り出す配列
  vector<SimNode*> mSimNodeMap;

  // FFR を納めた配列
  vector<SimFFR> mFFRArray;

  // SimNode のノード番号をキーにして対応する SimFFR を格納する配列
  vector<SimFFR*> mFFRMap;

  // イベントキュー
  EventQ mEventQ;

  // 全ての SimFault のリスト
  vector<unique_ptr<SimFault>> mFaultList;

  // TpgFault::id() をキーとして SimFault を格納する配列
  vector<SimFault*> mFaultMap;

  // 検出された故障を格納する配列
  vector<const TpgFault*> mDetFaultArray;

  // 故障ごとの検出パタンを収める配列
  // キーは TpgFault.id()
  unordered_map<SizeType, DiffBits> mDiffBitsMap;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIMX_H
