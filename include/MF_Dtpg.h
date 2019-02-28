#ifndef MF_DTPG_H
#define MF_DTPG_H

/// @file MF_Dtpg.h
/// @brief MF_Dtpg のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "Justifier.h"
#include "VidMap.h"

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MF_Dtpg MF_Dtpg.h "MF_Dtpg.h"
/// @brief 多重故障用の DTPG
//////////////////////////////////////////////////////////////////////
class MF_Dtpg
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] solver_type SATソルバの実装タイプ
  MF_Dtpg(const TpgNetwork& network,
	  FaultType fault_type,
	  const string& just_type,
	  const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~MF_Dtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault_list 対象の多重故障
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(const vector<const TpgFault*>& fault_list);

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  NodeValList
  get_sufficient_condition();

  /// @brief SATソルバに変数を割り当てる．
  SatVarId
  new_variable();

  /// @brief SATソルバに節を追加する．
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 一つの SAT問題を解く．
  /// @param[in] assumptions 値の決まっている変数のリスト
  /// @return 結果を返す．
  ///
  /// mSolver.solve() を呼び出すだけだが統計情報の更新を行っている．
  /// SATだった場合のモデルは mSatModel に格納される．
  SatBool3
  solve(const vector<SatLiteral>& assumptions);

  /// @brief SAT問題が充足可能か調べる．
  /// @param[in] assumptions 値の決まっている変数のリスト
  /// @return 結果を返す．
  ///
  /// solve() との違いは結果のモデルを保持しない．
  SatBool3
  check(const vector<SatLiteral>& assumptions);

  /// @brief 直前の solve() の結果からテストベクタを作る．
  /// @return 作成したテストベクタを返す．
  ///
  /// この関数では単純に外部入力の値を記録する．
  TestVector
  get_tv();

  /// @brief バックトレースを行う．
  /// @param[in] suf_cond 十分条件の割り当て
  /// @return テストパタンを返す．
  TestVector
  backtrace(const NodeValList& suf_cond);

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const;

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief ノード番号の最大値を返す．
  int
  max_node_id() const;

  /// @brief CNF 作成を開始する．
  void
  cnf_begin();

  /// @brief CNF 作成を終了する．
  void
  cnf_end();

  /// @brief 時間計測を開始する．
  void
  timer_start();

  /// @brief 時間計測を終了する．
  USTime
  timer_stop();

  /// @brief SATソルバを返す．
  SatSolver&
  solver();

  /// @brief 1時刻前の正常値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  hvar(const TpgNode* node) const;

  /// @brief 正常値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  gvar(const TpgNode* node) const;

  /// @brief 故障値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  fvar(const TpgNode* node) const;

  /// @brief 伝搬条件の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  dvar(const TpgNode* node) const;

  /// @brief 1時刻前の正常値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_hvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 正常値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_gvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 故障値値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_fvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 故障伝搬条件の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_dvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 1時刻前の正常値の変数マップを返す．
  const VidMap&
  hvar_map() const;

  /// @brief 正常値の変数マップを返す．
  const VidMap&
  gvar_map() const;

  /// @brief 故障値の変数マップを返す．
  const VidMap&
  fvar_map() const;

  /// @brief 1時刻前の正常値を得る．
  /// @param[in] node 対象のノード
  Val3
  hval(const TpgNode* node) const;

  /// @brief 正常値を得る．
  /// @param[in] node 対象のノード
  Val3
  gval(const TpgNode* node) const;

  /// @brief 故障値を得る．
  /// @param[in] node 対象のノード
  Val3
  fval(const TpgNode* node) const;

  /// @brief 直前の solve() の解を返す．
  const vector<SatBool3>&
  sat_model() const;

  /// @brief 起点となるノードのリストを返す．
  const vector<const TpgNode*>&
  root_node_list() const;

  /// @brief root_node() の TFO に含まれる出力のノードのリストを返す．
  const vector<const TpgNode*>&
  output_list() const;

  /// @brief 関係するノードのリストを返す．
  const vector<const TpgNode*>&
  cur_node_list() const;

  /// @brief 関係する１時刻前のノードのリストを返す．
  const vector<const TpgNode*>&
  prev_node_list() const;

  /// @brief 対象の部分回路の関係を表す変数を用意する．
  void
  prepare_vars();

  /// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
  void
  gen_good_cnf();

  /// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
  void
  gen_faulty_cnf();

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(const TpgNode* node);

  /// @brief TFO マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfoList に入れる．<br>
  /// 出力ノードの場合は mOutputList にも入れる．<br>
  /// すでにマークされていたら何もしない．
  void
  set_tfo_mark(const TpgNode* node);

  /// @brief TFI マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfiList に入れる．
  void
  set_tfi_mark(const TpgNode* node);

  /// @brief TFI2 マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfi2List に入れる．
  void
  set_tfi2_mark(const TpgNode* node);

  /// @brief root マークをつける．
  /// @param[in] node 対象のノード
  void
  set_root_mark(const TpgNode* node);

  /// @brief root マークを読む．
  /// @param[in] node 対象のノード
  bool
  root_mark(const TpgNode* node);

  /// @brief SATモデルから値を取り出す．
  /// @param[in] var 変数番号
  Val3
  get_val(SatVarId var) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 統計情報
  DtpgStats mStats;

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 故障伝搬の起点となるノードのリスト
  vector<const TpgNode*> mRootList;

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF 入れておくリスト
  vector<const TpgDff*> mDffList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mTfi2List;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 関係する１時刻目の外部入力ノードを入れておくリスト
  vector<const TpgNode*> mAuxInputList;

  // 関係する擬似外部入力ノードを入れておくリスト
  vector<const TpgNode*> mPPIList;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<ymuint8> mMarkArray;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // SATの解を保持する配列
  vector<SatBool3> mSatModel;

  // バックトレーサー
  Justifier mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  StopWatch mTimer;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 統計情報を得る．
inline
const DtpgStats&
MF_Dtpg::stats() const
{
  return mStats;
}

// @brief SATソルバに変数を割り当てる．
inline
SatVarId
MF_Dtpg::new_variable()
{
  return solver().new_variable();
}

// @brief SATソルバに節を追加する．
inline
void
MF_Dtpg::add_clause(const vector<SatLiteral>& lits)
{
  solver().add_clause(lits);
}

// @brief SATソルバを返す．
inline
SatSolver&
MF_Dtpg::solver()
{
  return mSolver;
}

// @brief 対象のネットワークを返す．
inline
const TpgNetwork&
MF_Dtpg::network() const
{
  return mNetwork;
}

// @brief 故障の種類を返す．
inline
FaultType
MF_Dtpg::fault_type() const
{
  return mFaultType;
}

// @brief ノード番号の最大値を返す．
inline
int
MF_Dtpg::max_node_id() const
{
  return network().node_num();
}

// @brief 起点となるノードを返す．
inline
const vector<const TpgNode*>&
MF_Dtpg::root_node_list() const
{
  return mRootList;
}

// @brief root_node() の TFO に含まれる出力のノードのリストを返す．
inline
const vector<const TpgNode*>&
MF_Dtpg::output_list() const
{
  return mOutputList;
}

// @brief 1時刻前の正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
MF_Dtpg::hvar(const TpgNode* node) const
{
  ASSERT_COND( mHvarMap(node) != kSatVarIdIllegal );

  return mHvarMap(node);
}

// @brief 正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
MF_Dtpg::gvar(const TpgNode* node) const
{
  return mGvarMap(node);
}

// @brief 故障値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
MF_Dtpg::fvar(const TpgNode* node) const
{
  return mFvarMap(node);
}

// @brief 伝搬条件の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
MF_Dtpg::dvar(const TpgNode* node) const
{
  return mDvarMap(node);
}

// @brief 1時刻前の正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
MF_Dtpg::set_hvar(const TpgNode* node,
		  SatVarId var)
{
  mHvarMap.set_vid(node, var);
}

// @brief 正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
MF_Dtpg::set_gvar(const TpgNode* node,
		  SatVarId var)
{
  mGvarMap.set_vid(node, var);
}

// @brief 故障値値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
MF_Dtpg::set_fvar(const TpgNode* node,
		  SatVarId var)
{
  mFvarMap.set_vid(node, var);
}

// @brief 故障伝搬条件の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
MF_Dtpg::set_dvar(const TpgNode* node,
		  SatVarId var)
{
  mDvarMap.set_vid(node, var);
}

// @brief 1時刻前の正常値の変数マップを返す．
inline
const VidMap&
MF_Dtpg::hvar_map() const
{
  return mHvarMap;
}

// @brief 正常値の変数マップを返す．
inline
const VidMap&
MF_Dtpg::gvar_map() const
{
  return mGvarMap;
}

// @brief 故障値の変数マップを返す．
inline
const VidMap&
MF_Dtpg::fvar_map() const
{
  return mFvarMap;
}

// @brief 1時刻前の正常値を得る．
// @param[in] node 対象のノード
inline
Val3
MF_Dtpg::hval(const TpgNode* node) const
{
  SatVarId var = hvar(node);
  return get_val(var);
}

// @brief 正常値を得る．
// @param[in] node 対象のノード
inline
Val3
MF_Dtpg::gval(const TpgNode* node) const
{
  SatVarId var = gvar(node);
  return get_val(var);
}

// @brief 故障値を得る．
// @param[in] node 対象のノード
inline
Val3
MF_Dtpg::fval(const TpgNode* node) const
{
  SatVarId var = fvar(node);
  return get_val(var);
}

// @brief 直前の solve() の解を返す．
inline
const vector<SatBool3>&
MF_Dtpg::sat_model() const
{
  return mSatModel;
}

// @brief SATモデルから値を取り出す．
// @param[in] var 変数番号
inline
Val3
MF_Dtpg::get_val(SatVarId var) const
{
  SatBool3 sat_val = mSatModel[var.val()];
  if ( sat_val == SatBool3::True ) {
    return Val3::_1;
  }
  else if ( sat_val == SatBool3::False ) {
    return Val3::_0;
  }
  else {
    return Val3::_X;
  }
}

// @brief TFO マークをつける．
inline
void
MF_Dtpg::set_tfo_mark(const TpgNode* node)
{
  int id = node->id();
  if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
    mMarkArray[id] |= 1U;
    mTfoList.push_back(node);
    if ( node->is_ppo() ) {
      mOutputList.push_back(node);
    }
    if ( mFaultType == FaultType::TransitionDelay ) {
      if ( node->is_primary_input() ) {
	mAuxInputList.push_back(node);
      }
    }
    else {
      if ( node->is_ppi() ) {
	mPPIList.push_back(node);
      }
    }
  }
}

// @brief TFI マークをつける．
inline
void
MF_Dtpg::set_tfi_mark(const TpgNode* node)
{
  int id = node->id();
  if ( (mMarkArray[id] & 3U) == 0U ) {
    mMarkArray[id] |= 2U;
    mTfiList.push_back(node);
    if ( mFaultType == FaultType::TransitionDelay ) {
      if ( node->is_dff_output() ) {
	mDffList.push_back(node->dff());
      }
      else if ( node->is_primary_input() ) {
	mAuxInputList.push_back(node);
      }
    }
    else {
      if ( node->is_ppi() ) {
	mPPIList.push_back(node);
      }
    }
  }
}

// @brief TFI2 マークをつける．
inline
void
MF_Dtpg::set_tfi2_mark(const TpgNode* node)
{
  int id = node->id();
  if ( ((mMarkArray[id] >> 2) & 1U) == 0U ) {
    mMarkArray[id] |= 4U;
    mTfi2List.push_back(node);
    if ( node->is_ppi() ) {
      mPPIList.push_back(node);
    }
  }
}

// @brief root マークをつける．
// @param[in] node 対象のノード
inline
void
MF_Dtpg::set_root_mark(const TpgNode* node)
{
  int id = node->id();
  mMarkArray[id] |= 8U;
}

// @brief root マークを読む．
// @param[in] node 対象のノード
inline
bool
MF_Dtpg::root_mark(const TpgNode* node)
{
  int id = node->id();
  return static_cast<bool>((mMarkArray[id] >> 3) & 1U);
}

END_NAMESPACE_DRUID

#endif // MF_DTPG_H
