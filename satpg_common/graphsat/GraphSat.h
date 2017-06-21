﻿#ifndef GRAPHSAT_H
#define GRAPHSAT_H

/// @file GraphSat.h
/// @brief GraphSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/Bool3.h"
#include "ym/Literal.h"
#include "ym/SatStats.h"
#include "ym/FragAlloc.h"
#include "ym/RandGen.h"
#include "ym/StopWatch.h"
#include "SatClause.h"
#include "SatReason.h"
#include "AssignList.h"
#include "Watcher.h"
#include "VarHeap.h"


BEGIN_NAMESPACE_YM_SATPG

class SatAnalyzer;

//////////////////////////////////////////////////////////////////////
/// @class Params GraphSat.h "GraphSat.h"
/// @brief GraphSat の挙動を制御するパラメータ
//////////////////////////////////////////////////////////////////////
struct Params
{
  /// @brief 変数の decay 値
  double mVarDecay;

  /// @brief 変数選択用の閾値
  double mVarFreq;

  /// @brief 学習節の decay 値
  double mClauseDecay;

  /// @brief phase-cache ヒューリスティックを使うとき true
  bool mPhaseCache;

  /// @brief watcher list の多い極性を選ぶヒューリスティックを使うとき true
  bool mWlPosi;

  /// @brief watcher list の少ない極性を選ぶヒューリスティックを使うとき true
  bool mWlNega;

  /// @brief LBD ヒューリスティックを使うとき true
  bool mUseLbd;

  /// @brief コンストラクタ
  Params() :
    mVarDecay(1.0),
    mVarFreq(0.0),
    mClauseDecay(1.0),
    mPhaseCache(true),
    mWlPosi(false),
    mWlNega(false),
    mUseLbd(false)
  {
  }

  /// @brief 値を指定したコンストラクタ
  Params(double var_decay,
	 double var_freq,
	 double clause_decay,
	 bool phase_cache,
	 bool wl_posi,
	 bool wl_nega,
	 bool use_lbd) :
    mVarDecay(var_decay),
    mVarFreq(var_freq),
    mClauseDecay(clause_decay),
    mPhaseCache(phase_cache),
    mWlPosi(wl_posi),
    mWlNega(!wl_posi && wl_nega),
    mUseLbd(use_lbd)
  {
  }

};


//////////////////////////////////////////////////////////////////////
/// @class GraphSat GraphSat.h "GraphSat.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class GraphSat
{
  friend class SatAnalyzer;

public:

  /// @brief コンストラクタ
  /// @param[in] option オプション文字列
  GraphSat(const string& option = string());

  /// @brief デストラクタ
  virtual
  ~GraphSat();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  virtual
  bool
  sane() const;

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  VarId
  new_variable();

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<Literal>& lits);

  /// @brief 節を追加する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  virtual
  void
  add_clause(ymuint lit_num,
	     const Literal* lits);

  /// @brief 1項の節(リテラル)を追加する．
  virtual
  void
  add_clause(Literal lit1);

  /// @brief 2項の節を追加する．
  virtual
  void
  add_clause(Literal lit1,
	     Literal lit2);

  /// @brief 3項の節を追加する．
  virtual
  void
  add_clause(Literal lit1,
	     Literal lit2,
	     Literal lit3);

  /// @brief 4項の節を追加する．
  virtual
  void
  add_clause(Literal lit1,
	     Literal lit2,
	     Literal lit3,
	     Literal lit4);

  /// @brief 5項の節を追加する．
  virtual
  void
  add_clause(Literal lit1,
	     Literal lit2,
	     Literal lit3,
	     Literal lit4,
	     Literal lit5);

  /// @brief PGraph の始点と終点をセットする．
  virtual
  void
  set_pgraph(TpgNode* source,
	     const vector<TpgNode*>& sink_list,
	     ymuint max_id);

  /// @brief SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  virtual
  Bool3
  solve(const vector<Literal>& assumptions,
	vector<Bool3>& model);

  /// @brief 学習節の整理を行なう．
  virtual
  void
  reduce_learnt_clause();

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  virtual
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  virtual
  ymuint
  variable_num() const;

  /// @brief 制約節の数を得る．
  virtual
  ymuint
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  virtual
  ymuint
  literal_num() const;

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  virtual
  ymuint64
  set_max_conflict(ymuint64 val);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  virtual
  void
  reg_msg_handler(SatMsgHandler* msg_handler);

  /// @brief 時間計測機能を制御する
  virtual
  void
  timer_on(bool enable);


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 探索を行う本体の関数
  /// @param[in] confl_limit 矛盾の制限値
  ///
  /// 矛盾の生起回数が confl_limit を越えたら kB3X を返す．
  Bool3
  search(ymuint confl_limit);

  /// @brief 割当てキューに基づいて implication を行う．
  SatReason
  implication();

  /// @brief バックトラックする
  /// @param[in] level バックトラックするレベル
  void
  backtrack(int level);

  /// @brief 次の割り当てを選ぶ．
  /// @note 割り当てられる変数がない場合には kLiteralX を返す．
  Literal
  next_decision();

  /// @brief 値の割当てか可能かチェックする．
  /// @param[in] lit 割り当てるリテラル
  /// @return 矛盾が起きたら false を返す．
  ///
  /// すでに同じ値が割り当てられていたらなにもしない．
  /// 割り当てには assign() を呼び出す．
  bool
  check_and_assign(Literal lit);

  /// @brief 値の割当てを行う．
  /// @param[in] lit 割り当てるリテラル
  /// @param[in] reason 割り当ての理由
  void
  assign(Literal lit,
	 SatReason reason = SatReason());

  /// @brief CNF を簡単化する．
  void
  sweep_clause();

  /// @brief 使われていない学習節を削除する．
  void
  cut_down();

  /// @brief add_clause() の下請け関数
  void
  add_clause_sub(ymuint lit_num);

  /// @brief 学習節を追加する．
  /// @note 追加するリテラルは mLearntLits に入れる．
  void
  add_learnt_clause();

  /// @brief mTmpLits を確保する．
  /// @param[in] lit_num リテラル数
  void
  alloc_lits(ymuint lit_num);

  /// @brief 新しい節を生成する．
  /// @param[in] lit_num リテラル数
  /// @param[in] learnt 学習節のとき true とするフラグ
  /// @param[in] lbd 学習節のときの literal block distance
  /// @note リテラルは mTmpLits に格納されている．
  SatClause*
  new_clause(ymuint lit_num,
	     bool learnt = false);

  /// @brief 節を削除する．
  /// @param[in] clause 削除する節
  void
  delete_clause(SatClause* clause);

  /// @brief watcher list を得る．
  /// @param[in] lit リテラル
  WatcherList&
  watcher_list(Literal lit);

  /// @brief Watcher を追加する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  add_watcher(Literal watch_lit,
	      SatReason reason);

  /// @brief watcher を削除する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  del_watcher(Literal watch_lit,
	      SatReason reason);

  /// @brief 変数1の評価を行う．
  /// @param[in] id 変数番号
  Bool3
  eval(VarId id) const;

  /// @brief literal の評価を行う．
  /// @param[in] l リテラル
  Bool3
  eval(Literal l) const;

  /// @brief 現在の decision level を返す．
  int
  decision_level() const;

  /// @brief 変数の decision level を返す．
  /// @param[in] varid 変数番号
  int
  decision_level(VarId varid) const;

  /// @brief LBD を計算する．
  ymuint
  calc_lbd(const SatClause* clause);

  /// @brief 変数の割り当て理由を返す．
  /// @param[in] varid 変数番号
  SatReason
  reason(VarId varid) const;

  /// @brief 学習節が使われているか調べる．
  /// @param[in] clause 対象の節
  bool
  is_locked(SatClause* clause) const;

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] var 変数番号
  void
  bump_var_activity(VarId var);

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity();

  /// @brief 学習節のアクティビティを増加させる．
  /// @param[in] clause 対象の学習節
  void
  bump_clause_activity(SatClause* clause);

  /// @brief 学習節のアクティビティを定率で減少させる．
  void
  decay_clause_activity();

  /// @brief 実際に変数に関するデータ構造を生成する．
  void
  alloc_var();

  /// @brief 変数に関する配列を拡張する．
  void
  expand_var();


private:
  //////////////////////////////////////////////////////////////////////
  // PGraph 関係の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief PGraph 上の mandatory assignment を求める．
  SatReason
  search_pgraph();

  /// @brief PGraph 上の optional assignment を求める．
  Literal
  find_path();

  /// @brief PGraph を DFS でたどる．
  int
  dfs_pgraph(TpgNode* node);

  /// @brief mark を消す．
  void
  dfs_clear(TpgNode* node);

  /// @brief PGraph 上のブロックリストから学習節を作る．
  SatReason
  add_pgraph_clause(const vector<TpgNode*>& block_list);

  /// @brief PGraph 上のブロックリストから学習節を作る．
  void
  add_pgraph_clause(const vector<TpgNode*>& block_list,
		    TpgNode* free_node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // PGraph の始点
  TpgNode* mSource;

  // PGraph の終点のリスト
  vector<TpgNode*> mSinkList;

  // PGraph の節点番号の最大値 + 1
  ymuint32 mMaxId;

  // PGraph 探索用のマーク
  vector<int> mMark;

  // block list
  vector<TpgNode*> mBlockList;

  // frontier list
  vector<TpgNode*> mFrontierList;

  bool mReached;

  ymuint32 mReachedLevel;

  // 解析器
  SatAnalyzer* mAnalyzer;

  // 正常の時に true となっているフラグ
  bool mSane;

  // SatClause のメモリ領域確保用のアロケータ
  FragAlloc mAlloc;

  // 制約節の配列
  vector<SatClause*> mConstrClause;

  // 二項制約節の数
  ymuint64 mConstrBinNum;

  // 制約節の総リテラル数 (二項制約節も含む)
  ymuint64 mConstrLitNum;

  // 学習節の配列
  vector<SatClause*> mLearntClause;

  // 二項学習節の数
  ymuint64 mLearntBinNum;

  // 学習節の総リテラル数 (二項制約節も含む)
  ymuint64 mLearntLitNum;

  // 変数の数
  ymuint32 mVarNum;

  // 前回の alloc_var で処理した時の変数の数
  ymuint32 mOldVarNum;

  // 変数関係の配列のサイズ
  ymuint32 mVarSize;

  // 値の配列
  // サイズは mVarSize
  ymuint8* mVal;

  // 値が割り当てられたときのレベルの配列
  // サイズは mVarSize
  int* mDecisionLevel;

  // 値が割り当てられた理由の配列
  // サイズは mVarSize
  SatReason* mReason;

  // watcher list の配列
  // サイズは mVarSize * 2
  WatcherList* mWatcherList;

  // 変数の極性ごとの重み
  // サイズは mVarSize * 2
  double* mWeightArray;

  // 変数のヒープ木
  VarHeap mVarHeap;

  // calc_lbd() 用の作業領域
  // サイズは decision_level()
  bool* mLbdTmp;

  // mLbdTmp のサイズ．
  ymuint32 mLbdTmpSize;

  // 矛盾の解析時にテンポラリに使用される節
  SatClause* mTmpBinClause;

  // search 開始時の decision level
  int mRootLevel;

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 前回の sweep 時の割り当て数
  ymuint64 mSweep_assigns;

  // 前回の sweep 時のリテラル数
  ymint64 mSweep_props;

  // 学習節のアクティビティの増加量
  double mClauseBump;

  // 学習節のアクティビティの減衰量
  double mClauseDecay;

  // 変数選択用乱数発生器
  RandGen mRandGen;

  // mTimer を使うとき true にするフラグ
  bool mTimerOn;

  // 時間計測器
  StopWatch mTimer;

  // 制御用パラメータ
  Params mParams;

  // restart 数
  ymuint mRestart;

  // 総コンフリクト数
  ymuint64 mConflictNum;

  // 総 decision 数
  ymuint64 mDecisionNum;

  // 総 implication 数
  ymuint64 mPropagationNum;

  // コンフリクト数の制限
  ymuint64 mConflictLimit;

  // 学習節の制限
  ymuint64 mLearntLimit;

  // mConflistLimit の最大値
  ymuint64 mMaxConflict;

  // メッセージハンドラのリスト
  list<SatMsgHandler*> mMsgHandlerList;

  // add_clause で一時的に利用するリテラル配列
  Literal* mTmpLits;

  // mTmpLits のサイズ
  ymuint32 mTmpLitsSize;

  // search() で用いられるリテラル配列
  vector<Literal> mLearntLits;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// watcher list を得る．
inline
WatcherList&
GraphSat::watcher_list(Literal lit)
{
  ymuint index = lit.index();
  return mWatcherList[index];
}

// Watcher を追加する．
inline
void
GraphSat::add_watcher(Literal watch_lit,
		   SatReason reason)
{
  watcher_list(watch_lit).add(Watcher(reason), mAlloc);
}

BEGIN_NONAMESPACE

inline
Bool3
conv_to_Bool3(ymuint8 x)
{
  int tmp = static_cast<int>(x) - 1;
  return static_cast<Bool3>(tmp);
}

inline
Bool3
cur_val(ymuint8 x)
{
  return conv_to_Bool3(x & 3U);
}

END_NONAMESPACE

// 変数の評価を行う．
inline
Bool3
GraphSat::eval(VarId id) const
{
  return cur_val(mVal[id.val()]);
}

// literal の評価を行う．
inline
Bool3
GraphSat::eval(Literal l) const
{
  ymuint index = l.index();
  ymuint x = mVal[index / 2] & 3U;
  ymuint inv = index & 1U;
  int d = 1 - (inv * 2);
  return static_cast<Bool3>((static_cast<int>(x) - 1) * d);
}

// 値の割当てか可能かチェックする．
// 矛盾が起きたら false を返す．
inline
bool
GraphSat::check_and_assign(Literal lit)
{
  Bool3 old_val = eval(lit);
  if ( old_val != kB3X ) {
    return old_val == kB3True;
  }
  assign(lit);
  return true;
}

BEGIN_NONAMESPACE

inline
ymuint8
conv_from_Bool3(Bool3 b)
{
  int tmp = static_cast<int>(b) + 1;
  return static_cast<ymuint8>(tmp);
}

END_NONAMESPACE

// 値の割当てを行う．
inline
void
GraphSat::assign(Literal lit,
	      SatReason reason)
{
  ymuint lindex = lit.index();
  ymuint vindex = lindex / 2;
  ymuint inv = lindex & 1U;
  ymuint8 x = 2 - inv * 2;
  mVal[vindex] = x;
  mDecisionLevel[vindex] = decision_level();
  mReason[vindex] = reason;

  // mAssignList に格納する．
  mAssignList.put(lit);
}

// 現在の decision level を返す．
inline
int
GraphSat::decision_level() const
{
  return mAssignList.cur_level();
}

// 変数の decision level を返す．
inline
int
GraphSat::decision_level(VarId varid) const
{
  return mDecisionLevel[varid.val()];
}

// 変数の割り当て理由を返す．
inline
SatReason
GraphSat::reason(VarId varid) const
{
  return mReason[varid.val()];
}

// @brief clase が含意の理由になっているか調べる．
inline
bool
GraphSat::is_locked(SatClause* clause) const
{
  // 直感的には分かりにくいが，節の最初のリテラルは
  // 残りのリテラルによって含意されていることになっている．
  // そこで最初のリテラルの変数の割り当て理由が自分自身か
  // どうかを調べれば clause が割り当て理由として用いられて
  // いるかわかる．
  return reason(clause->wl0().varid()) == SatReason(clause);
}

// @brief 変数のアクティビティを増加させる．
// @param[in] var 変数番号
inline
void
GraphSat::bump_var_activity(VarId var)
{
  mVarHeap.bump_var_activity(var);
}

// @brief 変数のアクティビティを定率で減少させる．
inline
void
GraphSat::decay_var_activity()
{
  mVarHeap.decay_var_activity();
}

END_NAMESPACE_YM_SATPG

#endif // GRAPHSAT_H
