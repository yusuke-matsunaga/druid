#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "FsimImpl.h"
#include "fsim_nsdef.h"
#include "PackedVal.h"
#include "PackedVal3.h"
#include "EventQ.h"
#include "SimFault.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "SimNodeList.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimFFR;
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
    const TpgNetwork& network, ///< [in] ネットワーク
    TpgFaultMgr& fmgr          ///< [in] 故障マネージャ
  );

  /// @brief デストラクタ
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all() override;

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    const TpgFault& f ///< [in] 対象の故障
  ) override;

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all() override;

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    const TpgFault& f ///< [in] 対象の故障
  ) override;


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
    const TpgFault& f     ///< [in] 対象の故障
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const NodeValList& assign_list, ///< [in] 値の割当リスト
    const TpgFault& f               ///< [in] 対象の故障
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  SizeType
  sppfp(
    const TestVector& tv ///< [in] テストベクタ
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  SizeType
  sppfp(
    const NodeValList& assign_list ///< [in] 値の割当リスト
  ) override;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  SizeType
  ppsfp() override;


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
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  void
  clear_patterns() override;

  /// @brief ppsfp 用のパタンを設定する．
  void
  set_pattern(
    SizeType pos,        ///< [in] 位置番号 ( 0 <= pos < PV_BITLEN )
    const TestVector& tv ///< [in] テストベクタ
  ) override;

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  TestVector
  get_pattern(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < PV_BITLEN )
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  SizeType
  det_fault_num() override
  {
    return mDetNum;
  }

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  TpgFault
  det_fault(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  ) override;

  /// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
  vector<TpgFault>
  det_fault_list() override;

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンを返す．
  PackedVal
  det_fault_pat(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < det_fault_num() )
  ) override
  {
    ASSERT_COND( pos >= 0 && pos < det_fault_num() );

    return mDetPatArray[pos];
  }

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
  const vector<PackedVal>&
  det_fault_pat_list() override
  {
    return mDetPatArray;
  }


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

  /// @brief FFR のリストを返す．
  const vector<SimFFR>&
  _ffr_list() const;

  /// @brief SPSFP故障シミュレーションの本体
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(
    const TpgFault& f ///< [in] 対象の故障
  );

  /// @brief SPPFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  SizeType
  _sppfp();

  /// @brief PPSFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  SizeType
  _ppsfp();

  /// @brief 正常値の計算を行う．
  void
  _calc_gval(
    const InputVals& input_vals ///< [in] 入力値
  );

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val();

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
  PackedVal
  _prop_sim(
    SimNode* root,     ///< [in] FFRの根のノード
    PackedVal obs_mask ///< [in] ビットマスク
  )
  {
    if ( root->is_output() ) {
      // 外部出力の場合は無条件で伝搬している．
      return PV_ALL1;
    }

    // それ以外はイベントドリヴンシミュレーションを行う．
    mEventQ.put_trigger(root, obs_mask, true);
    auto obs = mEventQ.simulate();

    return obs;
  }

  /// @brief FFR内の伝搬条件を求める．
  PackedVal
  _ffr_prop(
    SimFault* fault ///< [in] 対象の故障
  )
  {
    auto lobs = PV_ALL1;

    auto f_node = fault->origin_node();
    for ( auto node = f_node; !node->ffr_root(); ) {
      auto onode = node->fanout_top();
      auto pos = node->fanout_ipos();
      lobs &= onode->_calc_gobs(pos);
      node = onode;
    }

    return lobs;
  }

  /// @brief FFR内の故障シミュレーションを行う．
  PackedVal
  _fault_prop(
    SimFault* fault ///< [in] 対象の故障
  )
  {
    // 故障の活性化条件を求める．
    auto cval = fault->excitation_condition();

    // FFR 内の故障伝搬を行う．
    auto lobs = cval & _ffr_prop(fault);

#if FSIM_BSIDE
    // 1時刻前の条件を求める．
    auto pval = fault->previous_condition();
    lobs &= pval;
#endif

    return lobs;
  }

  /// @brief 結果の配列をクリアする．
  void
  clear_det_array()
  {
    mDetFaultArray.clear();
    mDetPatArray.clear();
    mDetNum = 0;
  }

  /// @brief 結果を追加する(sppfp用)．
  void
  add_det_array(
    SimFault* f ///< [in] 故障
  )
  {
    auto fid = f->id();
    mDetFaultArray.push_back(fid);
    ++ mDetNum;
  }

  /// @brief 結果を追加する(pppfp用)．
  void
  add_det_array(
    PackedVal pat, ///< [in] パタン
    SimFault* f    ///< [in] 故障
  )
  {
    auto fid = f->id();
    auto pat1 = pat & mPatMap;
    mDetFaultArray.push_back(fid);
    mDetPatArray.push_back(pat1);
    ++ mDetNum;
  }

  /// @brief 個々の故障の故障伝搬条件を計算する．
  /// @return 全ての故障の伝搬結果のORを返す．
  PackedVal
  _foreach_faults(
    const vector<SimFault*>& fault_list ///< [in] 故障のリスト
  );

  /// @brief シミュレーションを行って sppfp 用の _fault_sweep() を呼ぶ出す．
  void
  _do_simulation(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
  );

  /// @brief 故障をスキャンして結果をセットする(sppfp用)
  void
  _fault_sweep(
    const vector<SimFault*>& fault_list ///< [in] 故障のリスト
  );

  /// @brief 故障をスキャンして結果をセットする(ppsfp用)
  void
  _fault_sweep(
    const vector<SimFault*>& fault_list, ///< [in] 故障のリスト
    PackedVal pat                        ///< [in] 検出パタン
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

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

  // 故障マネージャ
  TpgFaultMgr& mFaultMgr;

  // 外部入力数
  SizeType mInputNum;

  // 外部出力数
  SizeType mOutputNum;

  // DFF数
  SizeType mDffNum;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  vector<SimNode*> mPPIList;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  vector<SimNode*> mPPOList;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // FFR 数
  SizeType mFFRNum;

  // FFR を納めた配列
  // サイズは mFFRNum
  vector<SimFFR> mFFRArray;

  // パタンの設定状況を表すビットベクタ
  PackedVal mPatMap;

  // mPatMap の最初の1のビット位置
  // 全て０の場合には PV_BITLEN が入る．
  SizeType mPatFirstBit;

  // パタンバッファ
  TestVector mPatBuff[PV_BITLEN];

  // イベントキュー
  EventQ mEventQ;

  // 故障数
  SizeType mFaultNum;

  // 全ての SimFault のリスト
  vector<SimFault*> mFaultList;

  // TpgFault::id() をキーとして SimFault を格納する配列
  vector<SimFault*> mFaultMap;

  // 検出された故障番号を格納する配列
  vector<SizeType> mDetFaultArray;

  // 故障を検出するビットパタンを格納する配列
  // サイズは常に mFaultNum
  vector<PackedVal> mDetPatArray;

  // 検出された故障数
  SizeType mDetNum;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIMX_H
