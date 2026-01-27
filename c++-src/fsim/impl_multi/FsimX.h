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
#include "SimNode.h"
#include "SimFFR.h"
#include "SimFault.h"
#include "types/TpgNode.h"
#include "types/TestVector.h"
#include "SimNodeList.h"
#include "SyncObj.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimEngine;

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
    const TpgNetwork& network,     ///< [in] ネットワーク
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
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const TestVector& tv, ///< [in] テストベクタ
    SizeType fid,         ///< [in] 対象の故障番号
    DiffBits& dbits       ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) override;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    SizeType fid,                  ///< [in] 対象の故障番号
    DiffBits& dbits                ///< [out] 出力ごとの伝搬状況を表すビットベクタ
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
    SizeType fid,                  ///< [in] 対象の故障番号
    DiffBits& dbits                ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  void
  sppfp(
    const TestVector& tv, ///< [in] テストベクタ
    cbtype1 callback      ///< [in] コールバック関数
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @return 検出された故障のリストを返す．
  void
  sppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback               ///< [in] コールバック関数
  ) override;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  void
  xsppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback               ///< [in] コールバック関数
                                   ///<      1番目の引数は検出された故障
                                   ///<      2番目の引数は出力の伝搬状況
  ) override;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 全パタンシミュレーションした場合に true を返す．
  ///
  /// callback 関数が false を返した場合にはこの関数も false を返す．
  void
  ppsfp(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
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
    if ( id >= ppi_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
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

  /// @brief ノード数を返す．
  SizeType
  node_num() const
  {
    return mNodeArray.size();
  }

  /// @brief 論理ノードのリストを返す．
  const std::vector<SimNode*>&
  logic_list() const
  {
    return mLogicArray;
  }

  /// @brief 最大レベルを返す．
  SizeType
  max_level() const
  {
    return mMaxLevel;
  }

  /// @brief DFF 数を返す．
  SizeType
  dff_num() const
  {
    return mDffNum;
  }

  /// @brief DFF の入力ノードを返す．
  SimNode*
  dff_input(
    SizeType id ///< [in] DFF番号 ( 0 <= id < dff_num() )
  ) const
  {
    if ( id >= dff_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mPPOList[id + mOutputNum];
  }

  /// @brief DFF の出力ノードを返す．
  SimNode*
  dff_output(
    SizeType id ///< [in] DFF番号 ( 0 <= id < dff_num() )
  ) const
  {
    if ( id >= dff_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mPPIList[id + mInputNum];
  }

  /// @brief FFR数を得る．
  SizeType
  ffr_num() const
  {
    return mFFRArray.size();
  }

  /// @brief FFRを得る．
  const SimFFR&
  ffr(
    SizeType id ///< [in] FFR番号 ( 0 <= id < ffr_num() )
  ) const
  {
    if ( id >= ffr_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mFFRArray[id];
  }

  /// @brief FFRの排列を得る．
  const std::vector<SimFFR>&
  ffr_array() const
  {
    return mFFRArray;
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

  /// @brief 対象の故障をセットする．
  void
  set_fault_list(
    const TpgFaultList& fault_list ///< [in] 対象の故障番号のリスト
  );


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
    auto level = input->level() + 1;
    auto node = make_gate(PrimType::Buff, level, {input->id()});
    node->set_output(output_id);
    return node;
  }

  /// @brief logic ノードを作る．
  SimNode*
  make_gate(
    PrimType type,
    SizeType level,
    const std::vector<SizeType>& inputs
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
  std::vector<std::unique_ptr<SimNode>> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  std::vector<SimNode*> mPPIList;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  std::vector<SimNode*> mPPOList;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  std::vector<SimNode*> mLogicArray;

  // TpgNode のノード番号から SimNode を取り出す配列
  std::vector<SimNode*> mSimNodeMap;

  // FFR を納めた配列
  std::vector<SimFFR> mFFRArray;

  // SimNode のノード番号をキーにして対応する SimFFR を格納する配列
  std::vector<SimFFR*> mFFRMap;

  // ノードの最大レベル
  SizeType mMaxLevel;

  // 全ての SimFault のリスト
  std::vector<std::unique_ptr<SimFault>> mFaultList;

  // TpgFault::id() をキーとして SimFault を格納する配列
  std::vector<SimFault*> mFaultMap;

  // 子スレッドとの同期用オブジェクト
  SyncObj mSyncObj;

  // 子スレッド用の SimEngine のリスト
  std::vector<std::unique_ptr<SimEngine>> mEngineList;

  // 子スレッドのリスト
  std::vector<std::thread> mThreadList;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIMX_H
