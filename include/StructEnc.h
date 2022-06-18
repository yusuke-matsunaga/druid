#ifndef STRUCTENC_H
#define STRUCTENC_H

/// @file StructEnc.h
/// @brief StructEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2016, 2017, 2018, 2022 Yusuke Matsunaga
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
  StructEnc(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] pe 故障の種類
    const SatSolverType& solver_type ///< [in] SATソルバの種類
    = SatSolverType{}
  );

  /// @brief デストラクタ
  ~StructEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const
  {
    return mFaultType;
  }

  /// @brief ノード番号の最大値を返す．
  SizeType
  max_node_id() const
  {
    return mMaxId;
  }

  /// @brief 変数マップを得る．
  ///
  /// 縮退故障モードの場合の時刻は 1
  const VidMap&
  var_map(
    int time  ///< [in] 時刻(0 or 1)
  ) const
  {
    return mVarMap[time & 1];
  }

  /// @brief 変数リテラルを得る．
  ///
  /// 縮退故障モードの場合の時刻は 1
  SatLiteral
  var(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻(0 or 1)
  ) const
  {
    return var_map(time)(node);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 節を作る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief fault cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  int
  add_simple_cone(
    const TpgNode* fnode, ///< [in] 故障のあるノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief fault cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  int
  add_simple_cone(
    const TpgNode* fnode, ///< [in] 故障のあるノード
    const TpgNode* bnode, ///< [in] ブロックノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief MFFC cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  int
  add_mffc_cone(
    const TpgMFFC& mffc, ///< [in] MFFC の情報
    bool detect		 ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief MFFC cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  int
  add_mffc_cone(
    const TpgMFFC& mffc,  ///< [in] MFFC の情報
    const TpgNode* bnode, ///< [in] ブロックノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief 故障を検出する条件を作る．
  vector<SatLiteral>
  make_fault_condition(
    const TpgFault* fault, ///< [in] 故障
    int cone_id 	   ///< [in] コーン番号
  );

  /// @brief 割当リストの内容を節に加える．
  ///
  /// すべて unit clause になる．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_assignments(
    const NodeValList& assignment  ///< [in] 割当リスト
  );

  /// @brief 割当リストの否定の節を加える．
  ///
  /// リテラル数が assignment.size() の1つの節を加える．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_negation(
    const NodeValList& assignment  ///< [in] 割当リスト
  );

  /// @brief 割当リストを仮定のリテラルに変換する．
  ///
  vector<SatLiteral>
  conv_to_literal_list(
    const NodeValList& assign_list ///< [in]  割当リスト
  );

  /// @brief 関係あるノードに変数を割り当てる．
  void
  make_vars();

  /// @brief 関係あるノードの入出力の関係を表すCNFを作る．
  void
  make_cnf();

  /// @brief node の TFI の部分に変数を割り当てる．
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  make_tfi_var(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻(0 or 1)
  );

  /// @brief node の TFI の CNF を作る．
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  make_tfi_cnf(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻(0 or 1)
  );

  /// @brief 変数マップを得る．
  ///
  /// 縮退故障モードの場合の時刻は 1
  VidMap&
  var_map(
    int time  ///< [in] 時刻(0 or 1)
  )
  {
    return mVarMap[time & 1];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // SAT 問題を解く関数
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックを行う．
  SatBool3
  check_sat();

  /// @brief 割当リストのもとでチェックを行う．
  SatBool3
  check_sat(
    const NodeValList& assign_list  ///< [in] 割当リスト
  );

  /// @brief 割当リストのもとでチェックを行う．
  SatBool3
  check_sat(
    const NodeValList& assign_list1, ///< [in] 割当リスト1
    const NodeValList& assign_list2  ///< [in] 割当リスト2
  );

  /// @brief 結果のなかで必要なものだけを取り出す．
  NodeValList
  extract(
    const SatModel& model, ///< [in] SAT のモデル
    const TpgFault* fault, ///< [in] 対象の故障
    int cone_id		   ///< [in] コーン番号
  );

  /// @brief 外部入力の値割り当てを求める．
  ///
  /// このクラスでの仕事はValMapに関する適切なオブジェクトを生成して
  /// justifier を呼ぶこと．
  TestVector
  justify(
    const SatModel& model,          ///< [in] SAT のモデル
    const NodeValList& assign_list, ///< [in] 値割り当てのリスト
    Justifier& justifier	    ///< [in] 正当化を行うファンクタ
  );

  /// @brief デバッグ用のフラグをセットする．
  void
  set_debug(
    int bits
  )
  {
    mDebugFlag = bits;
  }

  /// @brief デバッグ用のフラグを得る．
  int
  debug() const
  {
    return mDebugFlag;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の検出条件を割当リストに追加する．
  ///
  /// fault の影響がノードの出力に伝搬する条件を assumptions に加える．
  void
  add_fault_condition(
    const TpgFault* fault,    ///< [in] 故障
    NodeValList& assign_list  ///< [out] 条件を表す割当リスト
  );

  /// @brief FFR内の故障の伝搬条件を割当リストに追加する．
  ///
  /// * fault の影響が root_node の出力に伝搬する条件を assumptions に加える．
  /// * 内部で add_fault_condition() を呼ぶ．
  void
  add_ffr_condition(
    const TpgNode* root_node, ///< [in] FFRの根のノード
    const TpgFault* fault,    ///< [in] 故障
    NodeValList& assign_list  ///< [out] 条件を表す割当リスト
  );

  /// @brief 与えられたノード(のリスト)のTFIのリストを作る．
  void
  make_tfi_list(
    const vector<const TpgNode*>& node_list  ///< [in] ノードのリスト
  );

  /// @brief ノードの値割り当てに対応するリテラルを返す．
  SatLiteral
  nv_to_lit(
    NodeVal nv  ///< [in] ノードの値割り当て
  )
  {
    const TpgNode* node = nv.node();
    // node およびその TFI に関する節を追加する．
    // すでに節が作られていた場合にはなにもしない．
    int time = nv.time();
    return node_assign_to_lit(node, time, nv.val());
  }

  /// @brief ノードの値割り当てに対応するリテラルを返す．
  SatLiteral
  node_assign_to_lit(
    const TpgNode* node, ///< [in] ノード
    int time,		 ///< [in] 時刻 (0 or 1)
    bool val		 ///< [in] 値
  )
  {
    // node およびその TFI に関する節を追加する．
    // すでに節が作られていた場合にはなにもしない．
    make_tfi_cnf(node, time);
    bool inv = !val;
    return SatLiteral{var(node, time), inv};
  }

  /// @brief ノードに新しい変数を割り当てる．
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  set_new_var(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻(0 or 1)
  )
  {
    auto var = mSolver.new_variable();
    mSolver.freeze_literal(var);
    _set_var(node, time, var);
  }

  /// @brief ノードに変数を割り当てる．
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  _set_var(
    const TpgNode* node, ///< [in] ノード
    int time,		 ///< [in] 時刻(0 or 1)
    SatLiteral var	 ///< [in] 変数リテラル
  )
  {
    var_map(time).set_vid(node, var);
    int pos = time ? 0 : 1;
    mMark[node->id()][pos] = true;
  }

  /// @brief ノードの gvar が割り当てられているか調べる．
  ///
  /// 縮退故障モードの場合の時刻は 1
  bool
  var_mark(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻(0 or 1)
  ) const
  {
    int pos = time ? 0 : 1;
    return mMark[node->id()][pos];
  }

  /// @brief ノードの CNF が作成済みか調べる．
  ///
  /// 縮退故障モードの場合の時刻は 1
  bool
  cnf_mark(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻(0 or 1)
  ) const
  {
    int pos = time ? 2 : 3;
    return mMark[node->id()][pos];
  }

  /// @brief ノードに CNF マークをつける．
  ///
  /// 縮退故障モードの場合の時刻は 1
  void
  set_cnf_mark(
    const TpgNode* node, ///< [in] ノード
    int time		 ///< [in] 時刻(0 or 1)
  )
  {
    int pos = time ? 2 : 3;
    mMark[node->id()][pos] = true;
  }

  /// @brief mCurNodeList に登録済みのマークを得る．
  bool
  cur_mark(
    const TpgNode* node   ///< [in] ノード
  ) const
  {
    return mMark[node->id()][4];
  }

  /// @brief mCurNodeList に登録する．
  void
  add_cur_node(
    const TpgNode* node  ///< [in] ノード
  )
  {
    mCurNodeList.push_back(node);
    mMark[node->id()][4] = true;
  }

  /// @brief mPrevNodeList に登録する．
  bool
  prev_mark(
    const TpgNode* node  ///< [in] ノード
  ) const
  {
    return mMark[node->id()][5];
  }

  /// @brief mPrevNodeList に登録する．
  void
  add_prev_node(
    const TpgNode* node  ///< [in] ノード
  )
  {
    mPrevNodeList.push_back(node);
    mMark[node->id()][5] = true;
  }


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
  SizeType mMaxId;

  // 処理済みのノードの印
  // 0: gvar 割り当て済み
  // 1: hvar 割り当て済み
  // 2: CNF 作成済み
  // 3: 1時刻前の CNF 作成済み
  // 4: mCurNodeList に登録済み
  // 5: mPrevNodeList に登録済み
  vector<bitset<6>> mMark;

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

END_NAMESPACE_DRUID_STRUCTENC

#endif // STRUCTENC_H
