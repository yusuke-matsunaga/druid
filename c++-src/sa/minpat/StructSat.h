#ifndef STRUCTSAT_H
#define STRUCTSAT_H

/// @file StructSat.h
/// @brief StructSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "sa/sa_nsdef.h"
#include "VidMap.h"
#include "TpgNode.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID_SA

//////////////////////////////////////////////////////////////////////
/// @class StructSat StructSat.h "sa/StructSat.h"
/// @brief TpgNetwork の構造に基づく SAT ソルバ
//////////////////////////////////////////////////////////////////////
class StructSat
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  StructSat(ymuint max_node_id,
	    const string& sat_type = string(),
	    const string& sat_option = string(),
	    ostream* sat_outp = nullptr);

  /// @brief デストラクタ
  ~StructSat();


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

  /// @brief 変数マップを得る．
  const VidMap&
  var_map() const;

  /// @brief 変数番号を得る．
  /// @param[in] node ノード
  SatVarId
  var(const TpgNode* node) const;


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

  /// @brief ノードに正常値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] var 変数番号
  void
  set_var(const TpgNode* node,
	  SatVarId var);

  /// @brief ノードのマークを調べる．
  /// @param[in] node ノード
  bool
  mark(const TpgNode* node) const;

  /// @brief ノードにマークをつける．
  /// @param[in] node ノード
  void
  set_mark(const TpgNode* node);


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

  // 変数マップ
  VidMap mVarMap;

  // fanout cone のリスト
  vector<FoCone*> mFoConeList;

  // デバッグ用のフラグ
  ymuint mDebugFlag;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief SATソルバを返す．
inline
SatSolver&
StructSat::solver()
{
  return mSolver;
}

// @brief ノード番号の最大値を返す．
inline
ymuint
StructSat::max_node_id() const
{
  return mMaxId;
}

// @brief 変数マップを得る．
inline
const VidMap&
StructSat::var_map() const
{
  return mVarMap;
}

// @brief 変数番号を得る．
// @param[in] node ノード
inline
SatVarId
StructSat::var(const TpgNode* node) const
{
  return mVarMap(node);
}

// @brief ノードのマークを調べる．
// @param[in] node ノード
inline
bool
StructSat::mark(const TpgNode* node) const
{
  return mMark[node->id()];
}

// @brief ノードにマークをつける．
// @param[in] node ノード
inline
void
StructSat::set_mark(const TpgNode* node)
{
  mMark[node->id()] = true;
}

// @brief ノードに正常値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] var 変数番号
inline
void
StructSat::set_var(const TpgNode* node,
		   SatVarId var)
{
  mVarMap.set_vid(node, var);
}

// @brief チェックを行う．
//
// こちらは結果のみを返す．
inline
SatBool3
StructSat::check_sat()
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
StructSat::check_sat(const NodeValList& assign_list)
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
StructSat::check_sat(const NodeValList& assign_list1,
		     const NodeValList& assign_list2)
{
  vector<SatBool3> model;
  return check_sat(assign_list1, assign_list2, model);
}

// @brief デバッグ用のフラグをセットする．
inline
void
StructSat::set_debug(ymuint bits)
{
  mDebugFlag = bits;
}

// @brief デバッグ用のフラグを得る．
inline
ymuint
StructSat::debug() const
{
  return mDebugFlag;
}

END_NAMESPACE_DRUID_SA

#endif // STRUCTSAT_H
