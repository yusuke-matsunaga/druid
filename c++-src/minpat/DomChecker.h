#ifndef DOMCHECKER_H
#define DOMCHECKER_H

/// @file DomChecker.h
/// @brief DomChecker のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/StopWatch.h"

#include "VidMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomChecker DomChecker.h "DomChecker.h"
/// @brief 支配関係の判定を行うクラス
//////////////////////////////////////////////////////////////////////
class DomChecker
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] root 故障伝搬の起点となるノード
  /// @param[in] fault 故障伝搬をさせない故障
  /// @param[in] solver_type SATソルバの実装タイプ
  DomChecker(const TpgNetwork& network,
	     FaultType fault_type,
	     const TpgNode* root,
	     const TpgFault* fault,
	     const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~DomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @return 結果を返す．
  SatBool3
  check_detectable(const TpgFault* fault);

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const;

  /// @brief 値割り当てをリテラルに変換する．
  SatLiteral
  conv_to_literal(NodeVal node_val);

  /// @brief 値割り当てをリテラルのリストに変換する．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[out] assumptions 変換したリテラルを追加するリスト
  void
  conv_to_assumptions(const NodeValList& assign_list,
		      vector<SatLiteral>& assumptions);

  /// @brief SATソルバに変数を割り当てる．
  SatVarId
  new_variable();

  /// @brief SATソルバに節を追加する．
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 一つの SAT問題を解く．
  /// @param[in] assumptions 値の決まっている変数のリスト
  /// @param[out] model SAT モデル
  /// @return 結果を返す．
  ///
  /// mSolver.solve() を呼び出すだけだが統計情報の更新を行っている．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	vector<SatBool3>& model);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const;

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
  hvar(const TpgNode* node);

  /// @brief 正常値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  gvar(const TpgNode* node);

  /// @brief 故障値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  fvar(const TpgNode* node,
       int pos);

  /// @brief 伝搬条件の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  dvar(const TpgNode* node);

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
	   SatVarId var,
	   int pos);

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
  fvar_map(int pos) const;

  /// @brief 起点となるノードを返す．
  const TpgNode*
  root_node(int pos) const;

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

  /// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
  void
  gen_detect_cnf();

  /// @brief 故障の伝搬しない条件を表す CNF 式を作る．
  void
  gen_undetect_cnf();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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
  set_tfo_mark(const TpgNode* node,
	       int pos);

  /// @brief TFI マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfiList に入れる．
  void
  set_tfi_mark(const TpgNode* node);

  /// @brief prev TFI マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mPrevTfiList に入れる．
  void
  set_prev_tfi_mark(const TpgNode* node);


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

  // 故障
  const TpgFault* mFault;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot[2];

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList[2];

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF 入れておくリスト
  vector<const TpgDff*> mDffList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mPrevTfiList;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList[2];

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<ymuint8> mMarkArray;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap[2];

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

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
DomChecker::stats() const
{
  return mStats;
}

// @brief SATソルバに変数を割り当てる．
inline
SatVarId
DomChecker::new_variable()
{
  return solver().new_variable();
}

// @brief SATソルバに節を追加する．
inline
void
DomChecker::add_clause(const vector<SatLiteral>& lits)
{
  solver().add_clause(lits);
}

// @brief SATソルバを返す．
inline
SatSolver&
DomChecker::solver()
{
  return mSolver;
}

// @brief 対象のネットワークを返す．
inline
const TpgNetwork&
DomChecker::network() const
{
  return mNetwork;
}

// @brief ノード番号の最大値を返す．
inline
int
DomChecker::max_node_id() const
{
  return network().node_num();
}

// @brief 起点となるノードを返す．
inline
const TpgNode*
DomChecker::root_node(int pos) const
{
  return mRoot[pos];
}

// @brief 1時刻前の正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
DomChecker::hvar(const TpgNode* node)
{
  ASSERT_COND( mHvarMap(node) != kSatVarIdIllegal );

  return mHvarMap(node);
}

// @brief 正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
DomChecker::gvar(const TpgNode* node)
{
  return mGvarMap(node);
}

// @brief 故障値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
DomChecker::fvar(const TpgNode* node,
		 int pos)
{
  return mFvarMap[pos](node);
}

// @brief 伝搬条件の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
DomChecker::dvar(const TpgNode* node)
{
  return mDvarMap(node);
}

// @brief 1時刻前の正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
DomChecker::set_hvar(const TpgNode* node,
		     SatVarId var)
{
  mHvarMap.set_vid(node, var);
}

// @brief 正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
DomChecker::set_gvar(const TpgNode* node,
		     SatVarId var)
{
  mGvarMap.set_vid(node, var);
}

// @brief 故障値値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
DomChecker::set_fvar(const TpgNode* node,
		     SatVarId var,
		     int pos)
{
  mFvarMap[pos].set_vid(node, var);
}

// @brief 故障伝搬条件の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
DomChecker::set_dvar(const TpgNode* node,
		     SatVarId var)
{
  mDvarMap.set_vid(node, var);
}

// @brief 1時刻前の正常値の変数マップを返す．
inline
const VidMap&
DomChecker::hvar_map() const
{
  return mHvarMap;
}

// @brief 正常値の変数マップを返す．
inline
const VidMap&
DomChecker::gvar_map() const
{
  return mGvarMap;
}

// @brief 故障値の変数マップを返す．
inline
const VidMap&
DomChecker::fvar_map(int pos) const
{
  return mFvarMap[pos];
}

// @brief TFO マークをつける．
inline
void
DomChecker::set_tfo_mark(const TpgNode* node,
			 int pos)
{
  int id = node->id();
  ymuint8 mask = 1U << pos;
  if ( (mMarkArray[id] & mask) == 0U ) {
    mMarkArray[id] |= mask;
    mTfoList[pos].push_back(node);
    if ( node->is_ppo() ) {
      mOutputList[pos].push_back(node);
    }
    set_tfi_mark(node);
  }
}

// @brief TFI マークをつける．
inline
void
DomChecker::set_tfi_mark(const TpgNode* node)
{
  int id = node->id();
  ymuint8 mask = 4U;
  if ( (mMarkArray[id] & mask) == 0U ) {
    mMarkArray[id] |= mask;
    mTfiList.push_back(node);
    if ( mFaultType == FaultType::TransitionDelay && node->is_dff_output() ) {
      mDffList.push_back(node->dff());
    }
  }
}

// @brief TFI2 マークをつける．
inline
void
DomChecker::set_prev_tfi_mark(const TpgNode* node)
{
  int id = node->id();
  ymuint8 mask = 8U;
  if ( (mMarkArray[id] & mask) == 0U ) {
    mMarkArray[id] |= mask;
    mPrevTfiList.push_back(node);
  }
}

END_NAMESPACE_DRUID

#endif // DOMCHECKER_H
