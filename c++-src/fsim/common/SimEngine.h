#ifndef SIMENGINE_H
#define SIMENGINE_H

/// @file SimEngine.h
/// @brief SimEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "DiffBitsArray.h"
#include "FsimResultsRep.h"
#include "SimNode.h"
#include "SimNodeList.h"
#include "SimFFR.h"
#include "SimFault.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimEngine SimEngine.h "SimEngine.h"
/// @brief 故障シミュレーションの本体
///
/// キューに詰まれる要素は SimNode で，各々のノードはレベルを持つ．
/// このキューではレベルの小さい順に処理してゆく．同じレベルのノード
/// 間の順序は任意でよい．
//////////////////////////////////////////////////////////////////////
class SimEngine
{
public:

  /// @brief コンストラクタ
  ///
  /// 実際には fault_list の中で ffr_list の FFR に含まれる故障のみを
  /// 対象とする．
  /// 実は network は fault_list から取り出すことができるので冗長だが
  /// わかりやすさのため指定することにした．
  SimEngine(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list,       ///< [in] 対象の故障のリスト
    const std::vector<SizeType>& ffr_list ///< [in] 担当する FFR 番号のリスト
    = {}
  );

  /// @brief デストラクタ
  ~SimEngine() = default;


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

  /// @brief ノードを返す．
  SimNode*
  node(
    SizeType id ///< [in] ノード番号
  ) const
  {
    if ( id >= mSimNodeMap.size() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mSimNodeMap[id];
  }

  /// @brief ノードの属している FFR を返す．
  SimFFR*
  ffr(
    const SimNode* node
  )
  {
    return mFFRMap[node->id()];
  }

  /// @brief FFRのリストを返す．
  const std::vector<SimFFR>&
  ffr_list() const
  {
    return mFFRArray;
  }


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
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief 故障のスキップマークを得る．
  bool
  get_skip(
    SizeType fid ///< [in] 対象の故障番号
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションの本体
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief SPPFP故障シミュレーションの本体
  std::vector<SizeType>
  sppfp();

  /// @brief PPSFP故障シミュレーションの本体
  std::vector<std::vector<SizeType>>
  ppsfp(
    SizeType tv_num ///< [in] テストベクタ数
  );

  /// @brief SPSFP故障シミュレーションの本体
  /// @return 出力ごとの故障伝搬状況
  DiffBits
  spsfp2(
    SizeType fid ///< [in] 対象の故障番号
  );

  /// @brief SPPFP故障シミュレーションの本体
  FsimResultsRep*
  sppfp2();

  /// @brief PPSFP故障シミュレーションの本体
  std::vector<FsimResultsRep*>
  ppsfp2(
    SizeType tv_num ///< [in] テストベクタ数
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値の計算を行う．
  void
  calc_val(
    const TestVector& tv ///< [in] テストベクタ
  );

  /// @brief 値の計算を行う．
  void
  calc_val(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  );

  /// @brief 値の計算を行う．
  void
  calc_val(
    const AssignList& assign_list ///< [in] 外部入力の値割り当てのリスト
  );

#if FSIM_BSIDE
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
  );

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  SizeType
  calc_wsa(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    bool weighted              ///< [in] 重みフラグ
                               /// - false: ゲートの出力の遷移回数の和
                               /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)
                               ///          を掛けたものの和
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
    InputVector& i_vect, ///< [out] 外部入力のビットベクタ
    DffVector& f_vect    ///< [out] FFの値のビットベクタ
  );
#endif


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークの構造を設定する．
  void
  set_network(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    const std::vector<SizeType>& ffr_list ///< [in] 担当する FFR の番号のリスト
  );

  /// @brief 故障を設定する．
  void
  set_fault_list(
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  );

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
    const std::vector<SimNode*>& inputs
  );

  /// @brief 反転イベントノードを作る．
  SimNode*
  make_flip(
    SimNode* node
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp_simulation(
    const SimFFR* ffr_buff[],       ///< [in] FFR を入れた配列
    SizeType ffr_num,               ///< [in] FFR 数
    std::vector<SizeType>& det_list ///< [in] 検出された故障番号を格納するリスト
  );

  /// @brief sppfp 用のシミュレーションを行う．
  void
  _sppfp2_simulation(
    const SimFFR* ffr_buff[], ///< [in] FFR を入れた配列
    SizeType ffr_num          ///< [in] FFR 数
  );

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
    const SimFFR& ffr,              ///< [in] 対象の FFR
    PackedVal bitmask,              ///< [in] ビットマスク
    std::vector<SizeType>& det_list ///< [in] 検出された故障番号を格納するリスト
  )
  {
    auto& fault_list = ffr.fault_list();
    for ( auto ff: fault_list ) {
      if ( !ff->skip() && (ff->obs_mask() & bitmask) != PV_ALL0 ) {
	auto fid = ff->id();
	det_list.push_back(fid);
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
	mRes->add(fid, dbits);
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

  /// @brief sppfp2 の結果を作る．
  FsimResultsRep*
  _make_results();

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val()
  {
    for ( auto node: mLogicArray ) {
      node->set_calc_val();
    }
  }

#if FSIM_BSIDE
  /// @brief ノードの出力の(重み付き)信号遷移回数を求める．
  SizeType
  _calc_wsa(
    SimNode* node,
    bool weighted
  );
#endif

  /// @brief イベントドリブンシミュレーションを行う．
  /// @return 伝搬状況を返す．
  ///
  /// sppfp 用．反転イベントあり．
  PackedVal
  simulate();

  /// @brief イベントドリブンシミュレーションを行う．
  /// @return 伝搬状況を返す．
  ///
  /// ppsfp 用．反転イベントなし
  PackedVal
  simulate1();

  /// @brief イベントドリブンシミュレーションを行う．
  /// @return 出力における変化ビットを返す．
  ///
  /// sppfp2 用．反転イベントあり．
  DiffBitsArray
  simulate2();

  /// @brief イベントドリブンシミュレーションを行う．
  /// @return 出力における変化ビットを返す．
  ///
  /// ppsfp2 用．反転イベントなし
  DiffBitsArray
  simulate3();

  /// @brief ファンアウトのノードをキューに積む．
  void
  put_fanouts(
    SimNode* node ///< [in] 対象のノード
  )
  {
    auto no = node->fanout_num();
    if ( no == 1 ) {
      auto onode = node->fanout_top();
      put(onode);
    }
    else {
      for ( auto onode: node->fanout_list() ) {
	put(onode);
      }
    }
  }

  /// @brief キューに積む
  void
  put(
    SimNode* node ///< [in] 対象のノード
  )
  {
    if ( !node->in_queue() ) {
      node->set_queue();
      auto level = node->level();
      auto& w = mArray[level];
      node->mLink = w;
      w = node;
      if ( mNum == 0 || mCurLevel > level ) {
	mCurLevel = level;
      }
      ++ mNum;
    }
  }

  /// @brief キューから取り出す．
  /// @retval nullptr キューが空だった．
  SimNode*
  get()
  {
    if ( mNum > 0 ) {
      // mNum が正しければ mCurLevel がオーバーフローすることはない．
      for ( ; ; ++ mCurLevel ) {
	auto& w = mArray[mCurLevel];
	auto node = w;
	if ( node != nullptr ) {
	  node->clear_queue();
	  w = node->mLink;
	  -- mNum;
	  return node;
	}
      }
    }
    return nullptr;
  }

  /// @brief clear リストに追加する．
  void
  add_to_clear_list(
    SimNode* node,       ///< [in] 対象のノード
    FSIM_VALTYPE old_val ///< [in] 元の値
  )
  {
    mClearArray.push_back({node, old_val});
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 値を元に戻すための構造体
  struct RestoreInfo
  {
    // ノード
    SimNode* mNode;

    // 元の値
    FSIM_VALTYPE mVal;
  };


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
  // SimNode の所有権を持つ．
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

  // キューの先頭ノードの配列
  std::vector<SimNode*> mArray;

  // 現在のレベル．
  SizeType mCurLevel{0};

  // キューに入っているノード数
  SizeType mNum{0};

  // clear 用の情報の配列
  std::vector<RestoreInfo> mClearArray;

  // 全ての SimFault のリスト
  std::vector<std::unique_ptr<SimFault>> mFaultList;

  // 故障番号をキーとして SimFault を格納する配列
  std::vector<SimFault*> mFaultMap;

  // SPPFP2の結果
  FsimResultsRep* mRes;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMENGINE_H
