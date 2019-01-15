#ifndef STRUCTENC_H
#define STRUCTENC_H

/// @file StructEnc.h
/// @brief StructEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "structenc_nsdef.h"
#include "FaultType.h"
#include "VidMap.h"
#include "TpgNode.h"
#include "NodeValList.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

//////////////////////////////////////////////////////////////////////
/// @class StructEnc StructEnc.h "StructEnc.h"
/// @brief TpgNetwork の構造に基づく SAT ソルバ
//////////////////////////////////////////////////////////////////////
class StructEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  StructEnc(const TpgNetwork& network,
	    FaultType fault_type,
	    const SatSolverType& solver_type = SatSolverType("ymsat2"));

  /// @brief デストラクタ
  ~StructEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver();

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief ノード番号の最大値を返す．
  int
  max_node_id() const;

  /// @brief 変数マップを得る．
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  const VidMap&
  var_map(int time) const;

  /// @brief 変数番号を得る．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  SatVarId
  var(const TpgNode* node,
      int time) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 節を作る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief fault cone を追加する．
  /// @param[in] fnode 故障のあるノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  /// @return 作成されたコーン番号を返す．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  int
  add_simple_cone(const TpgNode* fnode,
		  bool detect);

  /// @brief fault cone を追加する．
  /// @param[in] fnode 故障のあるノード
  /// @param[in] bnode ブロックノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  int
  add_simple_cone(const TpgNode* fnode,
		  const TpgNode* bnode,
		  bool detect);

  /// @brief MFFC cone を追加する．
  /// @param[in] mffc MFFC の情報
  /// @param[in] detect 故障を検出する時に true にするフラグ
  /// @return 作成されたコーン番号を返す．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  int
  add_mffc_cone(const TpgMFFC& mffc,
		bool detect);

  /// @brief MFFC cone を追加する．
  /// @param[in] mffc MFFC の情報
  /// @param[in] bnode ブロックノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  int
  add_mffc_cone(const TpgMFFC& mffc,
		const TpgNode* bnode,
		bool detect);

  /// @brief 故障を検出する条件を作る．
  /// @param[in] fault 故障
  /// @param[in] cone_id コーン番号
  /// @param[out] assign_list 条件を表す割当リスト
  void
  make_fault_condition(const TpgFault* fault,
		       int cone_id,
		       vector<SatLiteral>& assumptions);

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

  /// @brief 関係あるノードに変数を割り当てる．
  void
  make_vars();

  /// @brief 関係あるノードの入出力の関係を表すCNFを作る．
  void
  make_cnf();

  /// @brief node の TFI の部分に変数を割り当てる．
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  make_tfi_var(const TpgNode* node,
	       int time);

  /// @brief node の TFI の CNF を作る．
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  make_tfi_cnf(const TpgNode* node,
	       int time);

  /// @brief 変数マップを得る．
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  VidMap&
  var_map(int time);


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
  ///
  /// こちらは結果のみを返す．
  SatBool3
  check_sat(const NodeValList& assign_list1,
	    const NodeValList& assign_list2);

  /// @brief 結果のなかで必要なものだけを取り出す．
  /// @param[in] model SAT のモデル
  /// @param[in] fault 対象の故障
  /// @param[in] cone_id コーン番号
  /// @param[out] 値の割り当て結果を入れるリスト
  NodeValList
  extract(const vector<SatBool3>& model,
	  const TpgFault* fault,
	  int cone_id);

  /// @brief 外部入力の値割り当てを求める．
  /// @param[in] model SAT のモデル
  /// @param[in] assign_list 値割り当てのリスト
  /// @param[in] justifier 正当化を行うファンクタ
  /// @param[out] testvect テストベクタ
  ///
  /// このクラスでの仕事はValMapに関する適切なオブジェクトを生成して
  /// justifier を呼ぶこと．
  void
  justify(const vector<SatBool3>& model,
	  const NodeValList& assign_list,
	  Justifier& justifier,
	  TestVector& testvect);

  /// @brief デバッグ用のフラグをセットする．
  void
  set_debug(int bits);

  /// @brief デバッグ用のフラグを得る．
  int
  debug() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の検出条件を割当リストに追加する．
  /// @param[in] fault 故障
  /// @param[out] assign_list 条件を表す割当リスト
  ///
  /// fault の影響がノードの出力に伝搬する条件を assumptions に加える．
  void
  add_fault_condition(const TpgFault* fault,
		      NodeValList& assign_list);

  /// @brief FFR内の故障の伝搬条件を割当リストに追加する．
  /// @param[in] root_node FFRの根のノード
  /// @param[in] fault 故障
  /// @param[out] assign_list 条件を表す割当リスト
  ///
  /// * fault の影響が root_node の出力に伝搬する条件を assumptions に加える．
  /// * 内部で add_fault_condition() を呼ぶ．
  void
  add_ffr_condition(const TpgNode* root_node,
		    const TpgFault* fault,
		    NodeValList& assign_list);

  /// @brief 与えられたノード(のリスト)のTFIのリストを作る．
  /// @param[in] node_list ノードのリスト
  void
  make_tfi_list(const vector<const TpgNode*>& node_list);

  /// @brief ノードの値割り当てに対応するリテラルを返す．
  /// @param[in] nv ノードの値割り当て
  SatLiteral
  nv_to_lit(NodeVal nv);

  /// @brief ノードの値割り当てに対応するリテラルを返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 (0 or 1)
  /// @param[in] val 値
  SatLiteral
  node_assign_to_lit(const TpgNode* node,
		     int time,
		     bool val);

  /// @brief ノードに新しい変数を割り当てる．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  set_new_var(const TpgNode* node,
	      int time);

  /// @brief ノードに変数を割り当てる．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  /// @param[in] var 変数
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  _set_var(const TpgNode* node,
	   int time,
	   SatVarId var);

  /// @brief ノードの gvar が割り当てられているか調べる．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  bool
  var_mark(const TpgNode* node,
	   int time) const;

  /// @brief ノードの CNF が作成済みか調べる．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  bool
  cnf_mark(const TpgNode* node,
	   int time) const;

  /// @brief ノードに CNF マークをつける．
  /// @param[in] node ノード
  /// @param[in] time 時刻(0 or 1)
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  set_cnf_mark(const TpgNode* node,
	       int time);

  /// @brief mCurNodeList に登録済みのマークを得る．
  /// @param[in] node ノード
  bool
  cur_mark(const TpgNode* node) const;

  /// @brief mCurNodeList に登録する．
  /// @param[in] node ノード
  void
  add_cur_node(const TpgNode* node);

  /// @brief mPrevNodeList に登録する．
  /// @param[in] node ノード
  bool
  prev_mark(const TpgNode* node) const;

  /// @brief mPrevNodeList に登録する．
  /// @param[in] node ノード
  void
  add_prev_node(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // SAT ソルバ
  SatSolver mSolver;

  // ノード番号の最大値
  int mMaxId;

  // 処理済みのノードの印
  // 0: gvar 割り当て済み
  // 1: hvar 割り当て済み
  // 2: CNF 作成済み
  // 3: 1時刻前の CNF 作成済み
  // 4: mCurNodeList に登録済み
  // 5: mPrevNodeList に登録済み
  vector<ymuint8> mMark;

  // 関係するノードのリスト
  vector<const TpgNode*> mCurNodeList;

  // 関係する１時刻前のノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // 変数マップ
  VidMap mVarMap[2];

  // propagation cone のリスト
  vector<PropCone*> mConeList;

  // デバッグ用のフラグ
  int mDebugFlag;

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

// @brief 故障の種類を返す．
inline
FaultType
StructEnc::fault_type() const
{
  return mFaultType;
}

// @brief ノード番号の最大値を返す．
inline
int
StructEnc::max_node_id() const
{
  return mMaxId;
}

// @brief ノードの値割り当てに対応するリテラルを返す．
// @param[in] nv ノードの値割り当て
inline
SatLiteral
StructEnc::nv_to_lit(NodeVal nv)
{
  const TpgNode* node = nv.node();
  // node およびその TFI に関する節を追加する．
  // すでに節が作られていた場合にはなにもしない．
  int time = nv.time();
  return node_assign_to_lit(node, time, nv.val());
}

// @brief ノードの値割り当てに対応するリテラルを返す．
// @param[in] node ノード
// @param[in] time 時刻 (0 or 1)
// @param[in] val 値
inline
SatLiteral
StructEnc::node_assign_to_lit(const TpgNode* node,
			      int time,
			      bool val)
{
  // node およびその TFI に関する節を追加する．
  // すでに節が作られていた場合にはなにもしない．
  make_tfi_cnf(node, time);
  bool inv = !val;
  return SatLiteral(var(node, time), inv);
}

// @brief 変数マップを得る．
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
const VidMap&
StructEnc::var_map(int time) const
{
  return mVarMap[time & 1];
}

// @brief 変数マップを得る．
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
VidMap&
StructEnc::var_map(int time)
{
  return mVarMap[time & 1];
}

// @brief 変数番号を得る．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
SatVarId
StructEnc::var(const TpgNode* node,
	       int time) const
{
  return var_map(time)(node);
}

// @brief ノードの変数が割り当てられているか調べる．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
bool
StructEnc::var_mark(const TpgNode* node,
		    int time) const
{
  int sft = time ? 0 : 1;
  return static_cast<bool>((mMark[node->id()] >> sft) & 1U);
}

// @brief ノードに新しい変数番号を割り当てる．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
void
StructEnc::set_new_var(const TpgNode* node,
		       int time)
{
  SatVarId var = mSolver.new_variable();
  mSolver.freeze_literal(SatLiteral(var));
  _set_var(node, time, var);
}

// @brief ノードに変数番号を割り当てる．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
void
StructEnc::_set_var(const TpgNode* node,
		    int time,
		    SatVarId var)
{
  var_map(time).set_vid(node, var);
  int sft = time ? 0 : 1;
  mMark[node->id()] |= (1U << sft);
}

// @brief ノードの CNF が作成済みか調べる．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
bool
StructEnc::cnf_mark(const TpgNode* node,
		    int time) const
{
  int sft = time ? 2 : 3;
  return static_cast<bool>((mMark[node->id()] >> sft) & 1U);
}

// @brief ノードに CNF マークをつける．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
inline
void
StructEnc::set_cnf_mark(const TpgNode* node,
			int time)
{
  int sft = time ? 2 : 3;
  mMark[node->id()] |= (1U << sft);
}

// @brief mCurNodeList に登録済みのマークを得る．
// @param[in] node ノード
inline
bool
StructEnc::cur_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMark[node->id()] >> 4) & 1U);
}

// @brief mCurNodeList に登録する．
// @param[in] node ノード
inline
void
StructEnc::add_cur_node(const TpgNode* node)
{
  mCurNodeList.push_back(node);
  mMark[node->id()] |= (1U << 4);
}

// @brief mPrevNodeList に登録する．
// @param[in] node ノード
inline
bool
StructEnc::prev_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMark[node->id()] >> 5) & 1U);
}

// @brief mPrevNodeList に登録する．
// @param[in] node ノード
inline
void
StructEnc::add_prev_node(const TpgNode* node)
{
  mPrevNodeList.push_back(node);
  mMark[node->id()] |= (1U << 5);
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
StructEnc::set_debug(int bits)
{
  mDebugFlag = bits;
}

// @brief デバッグ用のフラグを得る．
inline
int
StructEnc::debug() const
{
  return mDebugFlag;
}

END_NAMESPACE_DRUID_STRUCTENC

#endif // STRUCTENC_H
