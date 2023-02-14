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
  SizeType
  add_simple_cone(
    const TpgNode* fnode, ///< [in] 故障のあるノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  )
  {
    return add_simple_cone(fnode, nullptr, detect);
  }

  /// @brief fault cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  SizeType
  add_simple_cone(
    const TpgNode* fnode, ///< [in] 故障のあるノード
    const TpgNode* bnode, ///< [in] ブロックノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief MFFC cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  SizeType
  add_mffc_cone(
    const TpgMFFC& mffc, ///< [in] MFFC の情報
    bool detect		 ///< [in] 故障を検出する時に true にするフラグ
  )
  {
    return add_mffc_cone(mffc, nullptr, detect);
  }

  /// @brief MFFC cone を追加する．
  /// @return 作成されたコーン番号を返す．
  ///
  /// bnode までの故障伝搬条件を考える．
  SizeType
  add_mffc_cone(
    const TpgMFFC& mffc,  ///< [in] MFFC の情報
    const TpgNode* bnode, ///< [in] ブロックノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief 故障の伝搬条件を求める．
  vector<SatLiteral>
  make_prop_condition(
    const TpgNode* ffr_root, ///< [in] FFR の根のノード
    SizeType cone_id 	     ///< [in] コーン番号
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

#if 0
  /// @brief 結果のなかで必要なものだけを取り出す．
  NodeValList
  extract(
    const SatModel& model, ///< [in] SAT のモデル
    const TpgFault* fault, ///< [in] 対象の故障
    SizeType cone_id		   ///< [in] コーン番号
  );
#endif

  /// @brief 伝搬条件を求める．
  NodeValList
  extract_prop_condition(
    const TpgNode* ffr_root, ///< [in] FFR の根のノード
    SizeType cone_id,	     ///< [in] コーン番号
    const SatModel& model    ///< [in] SAT のモデル
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
    ymuint bits
  )
  {
    mDebugFlag = bits;
  }

  /// @brief デバッグ用のフラグを得る．
  ymuint
  debug() const
  {
    return mDebugFlag;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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
    //make_tfi_cnf(node, time);
    bool inv = !val;
    return var(node, time) * inv;
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
    auto var = mSolver.new_variable(true);
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

  // 関係するノードのリスト
  vector<const TpgNode*> mCurNodeList;

  // 関係する１時刻前のノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // 変数マップ
  VidMap mVarMap[2];

  // propagation cone のリスト
  vector<unique_ptr<PropCone>> mConeList;

  // デバッグ用のフラグ
  ymuint mDebugFlag;

};

END_NAMESPACE_DRUID_STRUCTENC

#endif // STRUCTENC_H