#ifndef TD_STRUCTSAT_H
#define TD_STRUCTSAT_H

/// @file td/StructEnc.h
/// @brief StructEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"
#include "VidMap.h"
#include "TpgNode.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID_TD

class FoCone;

//////////////////////////////////////////////////////////////////////
/// @class StructEnc StructEnc.h "sa/StructEnc.h"
/// @brief TpgNetwork の構造に基づく SAT ソルバ
//////////////////////////////////////////////////////////////////////
class StructEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  StructEnc(ymuint max_node_id,
	    const string& sat_type = string(),
	    const string& sat_option = string(),
	    ostream* sat_outp = nullptr);

  /// @brief デストラクタ
  ~StructEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver();

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief 1時刻前の変数マップを得る．
  const VidMap&
  hvar_map() const;

  /// @brief 現在の変数マップを得る．
  const VidMap&
  gvar_map() const;

  /// @brief 1時刻前の変数番号を得る．
  /// @param[in] node ノード
  SatVarId
  hvar(const TpgNode* node) const;

  /// @brief 現在の変数番号を得る．
  /// @param[in] node ノード
  SatVarId
  gvar(const TpgNode* node) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 節を作る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief fault cone を追加する．
  /// @param[in] fnode 故障のあるノード
  /// @param[in] detect 検出条件
  const FoCone*
  add_focone(const TpgNode* fnode,
	     Val3 detect);

  /// @brief fault cone を追加する．
  /// @param[in] fnode 故障のあるノード
  /// @param[in] bnode ブロックノード
  /// @param[in] detect 検出条件
  const FoCone*
  add_focone(const TpgNode* fnode,
	     const TpgNode* bnode,
	     Val3 detect);

  /// @brief fault cone を追加する．
  /// @param[in] fault 故障
  /// @param[in] detect 検出条件
  const FoCone*
  add_focone(const TpgFault* fault,
	     Val3 detect);

  /// @brief fault cone を追加する．
  /// @param[in] fault 故障
  /// @param[in] bnode ブロックノード
  /// @param[in] detect 検出条件
  const FoCone*
  add_focone(const TpgFault* fault,
	     const TpgNode* bnode,
	     Val3 detect);

  /// @brief fault cone を追加する．
  /// @param[in] fnode 故障のあるノード
  ///
  /// こちらは MFFC 内の故障を対象にする．
  const MffcCone*
  add_mffccone(const TpgNode* fnode);

  /// @brief 故障の検出条件を割当リストに追加する．
  /// @param[in] fault 故障
  /// @param[out] assignment 割当リスト
  ///
  /// fault の影響がノードの出力に伝搬する条件を assignment に加える．
  void
  add_fault_condition(const TpgFault* fault,
		      NodeValList& assignment);

  /// @brief FFR内の故障の伝搬条件を割当リストに追加する．
  /// @param[in] root_node FFRの根のノード
  /// @param[in] fault 故障
  /// @param[out] assignment 割当リスト
  ///
  /// fault の影響が root_node の出力に伝搬する条件を assignment に加える．
  void
  add_ffr_condition(const TpgNode* root_node,
		    const TpgFault* fault,
		    NodeValList& assignment);

  /// @brief 割当リストの内容を節に加える．
  /// @param[in] assignment 割当リスト
  ///
  /// すべて unit clause になる．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_assignments(const NodeValList& assignment);

  /// @brief 割当リストの否定の節を加える．
  /// @param[in] assignment 割当リスト
  ///
  /// リテラル数が assignment.size() の1つの節を加える．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_negation(const NodeValList& assignment);

  /// @brief 割当リストを仮定のリテラルに変換する．
  /// @param[in] assign_list 割当リスト
  /// @param[out] assumptions 仮定を表すリテラルのリスト
  ///
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  conv_to_assumption(const NodeValList& assign_list,
		     vector<SatLiteral>& assumptions);

  /// @brief node の TFI の CNF を作る．
  /// @param[in] node 対象のノード
  void
  make_tfi_cnf(const TpgNode* node);

  /// @brief node の TFI の CNF を作る．(1時刻前用)
  /// @param[in] node 対象のノード
  void
  make_tfi_cnf0(const TpgNode* node);


public:
  //////////////////////////////////////////////////////////////////////
  // SAT 問題を解く関数
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックを行う．
  /// @param[out] sat_model SATの場合の解
  SatBool3
  check_sat(vector<SatBool3>& sat_model);

  /// @brief チェックを行う．
  ///
  /// こちらは結果のみを返す．
  SatBool3
  check_sat();

  /// @brief 割当リストのもとでチェックを行う．
  /// @param[in] assign_list 割当リスト
  /// @param[out] sat_model SATの場合の解
  SatBool3
  check_sat(const NodeValList& assign_list,
	    vector<SatBool3>& sat_model);

  /// @brief 割当リストのもとでチェックを行う．
  /// @param[in] assign_list 割当リスト
  ///
  /// こちらは結果のみを返す．
  SatBool3
  check_sat(const NodeValList& assign_list);

  /// @brief 割当リストのもとでチェックを行う．
  /// @param[in] assign_list1, assign_list2 割当リスト
  /// @param[out] sat_model SATの場合の解
  SatBool3
  check_sat(const NodeValList& assign_list1,
	    const NodeValList& assign_list2,
	    vector<SatBool3>& sat_model);

  /// @brief 割当リストのもとでチェックを行う．
  /// @param[in] assign_list1, assign_list2 割当リスト
  /// @param[in] assign_list1, assign_list2 割当リスト
  ///
  /// こちらは結果のみを返す．
  SatBool3
  check_sat(const NodeValList& assign_list1,
	    const NodeValList& assign_list2);

  /// @brief デバッグ用のフラグをセットする．
  void
  set_debug(ymuint bits);

  /// @brief デバッグ用のフラグを得る．
  ymuint
  debug() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードに1時刻前の正常値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] var 変数番号
  void
  set_hvar(const TpgNode* node,
	   SatVarId var);

  /// @brief ノードに現在の正常値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] var 変数番号
  void
  set_gvar(const TpgNode* node,
	   SatVarId var);

  /// @brief ノードのマークを調べる．
  /// @param[in] node ノード
  bool
  mark(const TpgNode* node) const;

  /// @brief ノードにマークをつける．
  /// @param[in] node ノード
  void
  set_mark(const TpgNode* node);

  /// @brief ノードのマークを調べる．
  /// @param[in] node ノード
  bool
  mark0(const TpgNode* node) const;

  /// @brief ノードにマークをつける．
  /// @param[in] node ノード
  void
  set_mark0(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT ソルバ
  SatSolver mSolver;

  // ノード番号の最大値
  ymuint mMaxId;

  // 処理済みのノードの印
  vector<bool> mMark;

  // 処理済みのノードの印(１時刻前用)
  vector<bool> mMark0;

  // 1時刻前の変数マップ
  VidMap mHvarMap;

  // 現在の変数マップ
  VidMap mGvarMap;

  // fanout cone のリスト
  vector<FoCone*> mFoConeList;

  // MFFC cone のリスト
  vector<MffcCone*> mMffcConeList;

  // デバッグ用のフラグ
  ymuint mDebugFlag;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief SATソルバを返す．
inline
SatSolver&
StructEnc::solver()
{
  return mSolver;
}

// @brief ノード番号の最大値を返す．
inline
ymuint
StructEnc::max_node_id() const
{
  return mMaxId;
}

// @brief 1時刻前の変数マップを得る．
inline
const VidMap&
StructEnc::hvar_map() const
{
  return mHvarMap;
}

// @brief 現在の変数マップを得る．
inline
const VidMap&
StructEnc::gvar_map() const
{
  return mGvarMap;
}

// @brief 1時刻前の変数番号を得る．
// @param[in] node ノード
inline
SatVarId
StructEnc::hvar(const TpgNode* node) const
{
  return mHvarMap(node);
}

// @brief 現在の変数番号を得る．
// @param[in] node ノード
inline
SatVarId
StructEnc::gvar(const TpgNode* node) const
{
  return mGvarMap(node);
}

// @brief ノードのマークを調べる．
// @param[in] node ノード
inline
bool
StructEnc::mark(const TpgNode* node) const
{
  return mMark[node->id()];
}

// @brief ノードにマークをつける．
// @param[in] node ノード
inline
void
StructEnc::set_mark(const TpgNode* node)
{
  mMark[node->id()] = true;
}

// @brief ノードのマークを調べる．
// @param[in] node ノード
inline
bool
StructEnc::mark0(const TpgNode* node) const
{
  return mMark0[node->id()];
}

// @brief ノードにマークをつける．
// @param[in] node ノード
inline
void
StructEnc::set_mark0(const TpgNode* node)
{
  mMark0[node->id()] = true;
}

// @brief ノードに1時刻前の正常値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] var 変数番号
inline
void
StructEnc::set_hvar(const TpgNode* node,
		    SatVarId var)
{
  mHvarMap.set_vid(node, var);
}

// @brief ノードに現在の正常値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] var 変数番号
inline
void
StructEnc::set_gvar(const TpgNode* node,
		    SatVarId var)
{
  mGvarMap.set_vid(node, var);
}

// @brief チェックを行う．
//
// こちらは結果のみを返す．
inline
SatBool3
StructEnc::check_sat()
{
  vector<SatBool3> model;
  return check_sat(model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] assign_list 割当リスト
//
// こちらは結果のみを返す．
inline
SatBool3
StructEnc::check_sat(const NodeValList& assign_list)
{
  vector<SatBool3> model;
  return check_sat(assign_list, model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] gval_cnf 正常回路用のデータ構造
// @param[in] assign_list1, assign_list2 割当リスト
//
// こちらは結果のみを返す．
inline
SatBool3
StructEnc::check_sat(const NodeValList& assign_list1,
		     const NodeValList& assign_list2)
{
  vector<SatBool3> model;
  return check_sat(assign_list1, assign_list2, model);
}

// @brief デバッグ用のフラグをセットする．
inline
void
StructEnc::set_debug(ymuint bits)
{
  mDebugFlag = bits;
}

// @brief デバッグ用のフラグを得る．
inline
ymuint
StructEnc::debug() const
{
  return mDebugFlag;
}

END_NAMESPACE_DRUID_TD

#endif // TD_STRUCTSAT_H
