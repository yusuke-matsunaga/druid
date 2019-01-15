#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2013, 2016, 2017 Yusuke Matsunaga
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
  /// @param[in] network ネットワーク
  FSIM_CLASSNAME (const TpgNetwork& network);

  /// @brief デストラクタ
  virtual
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  /// @param[in] f 対象の故障
  virtual
  void
  set_skip(const TpgFault* f);

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  /// @param[in] f 対象の故障
  virtual
  void
  clear_skip(const TpgFault* f);


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const TestVector& tv,
	const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const NodeValList& assign_list,
	const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  int
  sppfp(const TestVector& tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  int
  sppfp(const NodeValList& assign_list);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  int
  ppsfp();


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
  virtual
  int
  calc_wsa(const TestVector& tv,
	   bool weighted);

  /// @brief 状態を設定する．
  /// @param[in] i_vect 外部入力のビットベクタ
  /// @param[in] f_vect FFの値のビットベクタ
  virtual
  void
  set_state(const InputVector& i_vect,
	    const DffVector& f_vect);

  /// @brief 状態を取得する．
  /// @param[in] i_vect 外部入力のビットベクタ
  /// @param[in] f_vect FFの値のビットベクタ
  virtual
  void
  get_state(InputVector& i_vect,
	    DffVector& f_vect);

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  /// @param[in] i_vect 外部入力のビットベクタ
  virtual
  int
  calc_wsa(const InputVector& i_vect,
	   bool weighted);


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  virtual
  void
  clear_patterns();

  /// @brief ppsfp 用のパタンを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  /// @param[in] tv テストベクタ
  virtual
  void
  set_pattern(int pos,
	      const TestVector& tv);

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  virtual
  TestVector
  get_pattern(int pos);


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  virtual
  int
  det_fault_num();

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  const TpgFault*
  det_fault(int pos);

  /// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
  virtual
  Array<const TpgFault*>
  det_fault_list();

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  PackedVal
  det_fault_pat(int pos);

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
  virtual
  Array<PackedVal>
  det_fault_pat_list();


public:
  //////////////////////////////////////////////////////////////////////
  // 内部のデータ構造にアクセスする関数
  // InputVals が用いる．
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を返す．
  int
  input_num() const;

  /// @brief PPI数を返す．
  int
  ppi_num() const;

  /// @brief PPI のノードを返す．
  /// @param[in] id PPI番号 ( 0 <= id < ppi_num() )
  SimNode*
  ppi(int id) const;

  /// @brief 外部入力ノードのリストを返す．
  Array<SimNode*>
  input_list() const;

  /// @brief DFFの出力ノードのリストを返す．
  Array<SimNode*>
  dff_output_list() const;

  /// @brief PPI のノードのリストを返す．
  Array<SimNode*>
  ppi_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  /// @param[in] network ネットワーク
  ///
  /// 全ての故障のスキップマークはクリアされる．
  void
  set_network(const TpgNetwork& network);

  /// @brief FFR のリストを返す．
  Array<SimFFR>
  _ffr_list() const;

  /// @brief SPSFP故障シミュレーションの本体
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(const TpgFault* f);

  /// @brief SPPFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  int
  _sppfp();

  /// @brief PPSFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  int
  _ppsfp();

  /// @brief 正常値の計算を行う．
  /// @param[in] input_vals 入力値
  void
  _calc_gval(const InputVals& input_vals);

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val();

  /// @brief ノードの出力の(重み付き)信号遷移回数を求める．
  int
  _calc_wsa(SimNode* node,
	    bool weighted);

  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @param[in] root FFRの根のノード
  /// @param[in] obs_mask ビットマスク
  /// @return 伝搬したビットに1を立てたビットベクタ
  ///
  /// obs_mask が0のビットのイベントはマスクされる．
  PackedVal
  _prop_sim(SimNode* root,
	    PackedVal obs_mask);

  /// @brief FFR内の伝搬条件を求める．
  /// @param[in] fault 対象の故障
  PackedVal
  _ffr_prop(SimFault* fault);

  /// @brief FFR内の故障シミュレーションを行う．
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_prop(SimFault* fault);

  /// @brief 個々の故障の故障伝搬条件を計算する．
  /// @param[in] fault_list 故障のリスト
  /// @return 全ての故障の伝搬結果のORを返す．
  PackedVal
  _foreach_faults(const vector<SimFault*>& fault_list);

  /// @brief 故障の活性化条件を求める．
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_cond(SimFault* fault);

  /// @brief 故障の活性化条件を求める．(遷移故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_prev_cond(SimFault* fault);

  /// @brief シミュレーションを行って sppfp 用の _fault_sweep() を呼ぶ出す．
  /// @param[in] ffr_buf FFR を入れた配列
  /// @param[in] ffr_num FFR 数
  void
  _do_simulation(const SimFFR* ffr_buff[],
  int ffr_num);

  /// @brief 故障をスキャンして結果をセットする(sppfp用)
  /// @param[in] fault_list 故障のリスト
  void
  _fault_sweep(const vector<SimFault*>& fault_list);

  /// @brief 故障をスキャンして結果をセットする(ppsfp用)
  /// @param[in] fault_list 故障のリスト
  /// @param[in] pat 検出パタン
  void
  _fault_sweep(const vector<SimFault*>& fault_list,
	       PackedVal pat);


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在保持している SimNode のネットワークを破棄する．
  /// 内部で clear_faults() を呼ぶ．
  void
  clear();

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

  /// @brief logic ノードを作る．
  SimNode*
  make_gate(GateType type,
	    const vector<SimNode*>& inputs);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部入力数
  int mInputNum;

  // 外部出力数
  int mOutputNum;

  // DFF数
  int mDffNum;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  SimNode** mPPIArray;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  SimNode** mPPOArray;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // ブロードサイド方式用の１時刻前の値を保持する配列
  // サイズは mNodeArray.size()
  FSIM_VALTYPE* mPrevValArray;

  // FFR 数
  int mFFRNum;

  // FFR を納めた配列
  // サイズは mFFRNum
  SimFFR* mFFRArray;

  // SimNode->id() をキーにして所属する FFR を納めた配列
  SimFFR** mFFRMap;

  // パタンの設定状況を表すビットベクタ
  PackedVal mPatMap;

  // mPatMap の最初の1のビット位置
  // 全て０の場合には kPvBitLen が入る．
  int mPatFirstBit;

  // パタンバッファ
  TestVector mPatBuff[kPvBitLen];

  // イベントキュー
  EventQ mEventQ;

  // 故障数
  int mFaultNum;

  // 故障シミュレーション用の故障の配列
  // サイズは mFaultNum
  SimFault* mSimFaults;

  // TpgFault::id() をキーとして SimFault を格納する配列
  SimFault** mFaultArray;

  // 検出された故障を格納する配列
  // サイズは常に mFaultNum
  const TpgFault** mDetFaultArray;

  // 故障を検出するビットパタンを格納する配列
  // サイズは常に mFaultNum
  PackedVal* mDetPatArray;

  // 検出された故障数
  int mDetNum;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 外部入力数を返す．
inline
int
FSIM_CLASSNAME::input_num() const
{
  return mInputNum;
}

// @brief PPI数を返す．
inline
int
FSIM_CLASSNAME::ppi_num() const
{
  return mInputNum + mDffNum;
}

// @brief PPI のノードを返す．
// @param[in] id PPI番号 ( 0 <= id < ppi_num() )
inline
SimNode*
FSIM_CLASSNAME::ppi(int id) const
{
  ASSERT_COND( id >= 0 && id < ppi_num() );

  return mPPIArray[id];
}

// @brief 外部入力ノードのリストを返す．
inline
Array<SimNode*>
FSIM_CLASSNAME::input_list() const
{
  return Array<SimNode*>(mPPIArray, 0, input_num());
}

// @brief DFFの出力ノードのリストを返す．
inline
Array<SimNode*>
FSIM_CLASSNAME::dff_output_list() const
{
  return Array<SimNode*>(mPPIArray, input_num(), ppi_num());
}

// @brief PPI のノードのリストを返す．
inline
Array<SimNode*>
FSIM_CLASSNAME::ppi_list() const
{
  return Array<SimNode*>(mPPIArray, 0, ppi_num());
}

// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
inline
int
FSIM_CLASSNAME::det_fault_num()
{
  return mDetNum;
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
inline
const TpgFault*
FSIM_CLASSNAME::det_fault(int pos)
{
  ASSERT_COND( pos >= 0 && pos < det_fault_num() );

  return mDetFaultArray[pos];
}

// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
inline
Array<const TpgFault*>
FSIM_CLASSNAME::det_fault_list()
{
  return Array<const TpgFault*>(mDetFaultArray, 0, mDetNum);
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
inline
PackedVal
FSIM_CLASSNAME::det_fault_pat(int pos)
{
  ASSERT_COND( pos >= 0 && pos < det_fault_num() );

  return mDetPatArray[pos];
}

inline
Array<PackedVal>
FSIM_CLASSNAME::det_fault_pat_list()
{
  return Array<PackedVal>(mDetPatArray, 0, mDetNum);
}

BEGIN_NONAMESPACE

// 故障の活性化条件を返す．
inline
PackedVal
_fault_diff(const TpgFault* f,
	    FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return ( f->val() == 1 ) ? ~val : val;
#elif FSIM_VAL3
  return ( f->val() == 1 ) ? val.val0() : val.val1();
#endif
}

// 遷移故障の初期化条件を返す．
inline
PackedVal
_fault_eq(const TpgFault* f,
	  FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return ( f->val() == 1 ) ? val : ~val;
#elif FSIM_VAL3
  return ( f->val() == 1 ) ? val.val1() : val.val0();
#endif
}

END_NONAMESPACE

// @brief FFR内の故障シミュレーションを行う．
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_fault_prop(SimFault* fault)
{
  // 故障の活性化条件を求める．
  auto cval = _fault_cond(fault);

  // FFR 内の故障伝搬を行う．
  auto lobs = _ffr_prop(fault);

#if FSIM_SA
  return cval & lobs;
#elif FSIM_TD
  // 1時刻前の条件を求める．
  auto pval = _fault_prev_cond(fault);

  return lobs & cval & pval;
#else
  return 0UL;
#endif
}

// @brief FFR内の伝搬条件を求める．
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_ffr_prop(SimFault* fault)
{
  auto lobs = kPvAll1;

  auto f_node = fault->mNode;
  for ( auto node = f_node; !node->is_ffr_root(); ) {
    auto onode = node->fanout_top();
    auto pos = node->fanout_ipos();
    lobs &= onode->_calc_gobs(pos);
    node = onode;
  }

  auto f = fault->mOrigF;
  if ( f->is_branch_fault() ) {
    // 入力の故障
    auto ipos = fault->mIpos;
    lobs &= f_node->_calc_gobs(ipos);
  }

  return lobs;
}

// @brief 故障の活性化条件を求める．(縮退故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_fault_cond(SimFault* fault)
{
  // 故障の入力側のノードの値
  auto ival = fault->mInode->val();

  // それが故障値と異なっていることが条件
  auto valdiff = _fault_diff(fault->mOrigF, ival);

  return valdiff;
}

// @brief 故障の活性化条件を求める．(遷移故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_fault_prev_cond(SimFault* fault)
{
  // １時刻前の値が故障値と同じである必要がある．
  auto pval = mPrevValArray[fault->mInode->id()];
  auto valeq = _fault_eq(fault->mOrigF, pval);

  return valeq;
}

// @brief FFR の根から故障伝搬シミュレーションを行う．
// @param[in] root FFRの根のノード
// @param[in] obs_mask ビットマスク
// @return 伝搬したビットに1を立てたビットベクタ
//
// obs_mask が0のビットのイベントはマスクされる．
inline
PackedVal
FSIM_CLASSNAME::_prop_sim(SimNode* root,
			  PackedVal obs_mask)
{
  if ( root->is_output() ) {
    // 外部出力の場合は無条件で伝搬している．
    return kPvAll1;
  }

  // それ以外はイベントドリヴンシミュレーションを行う．
  mEventQ.put_trigger(root, obs_mask, true);
  auto obs = mEventQ.simulate();

  return obs;
}

END_NAMESPACE_DRUID_FSIM

#endif // FSIMX_H
