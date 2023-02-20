#ifndef DTPGENGINE2_H
#define DTPGENGINE2_H

/// @file DtpgEngine2.h
/// @brief DtpgEngine2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "Val3.h"
#include "VidMap.h"

#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine2 DtpgEngine2.h "DtpgEngine2.h"
/// @brief DTPG の基本的な処理を行うクラス
///
/// 基本的には get_pattern(fault) で対象のテストパタンを求める．
//////////////////////////////////////////////////////////////////////
class DtpgEngine2
{
public:

  /// @brief コンストラクタ
  DtpgEngine2(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgNode* root,	     ///< [in] 故障伝搬の起点となるノード
    bool make_dchain,                ///< [in] dchain を作る時 true にする．
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
  );

  /// @brief デストラクタ
  ~DtpgEngine2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF の生成を行う．
  ///
  /// root からいずれかの外部出力へ故障の影響が伝搬する
  /// 条件を表す CNF を作る．
  void
  make_cnf();

  /// @brief 値割り当てをリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeVal node_val ///< [in] 値割当リスト
  );

  /// @brief 値割り当てをリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    const NodeValList& assign_list ///< [in] 値の割り当てリスト
  )
  {
    vector<SatLiteral> ans_list;
    add_to_literal_list(assign_list, ans_list);
    return ans_list;
  }

  /// @brief 値割り当てをリテラルのリストに追加する．
  void
  add_to_literal_list(
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    vector<SatLiteral>& lit_list    ///< [out] 変換したリテラルを追加するリスト
  );

  /// @brief SATソルバに変数を割り当てる．
  SatLiteral
  new_variable()
  {
    return solver().new_variable();
  }

  /// @brief SATソルバに節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] 節を構成するリテラルのリスト
  )
  {
    solver().add_clause(lits);
  }

  /// @brief SATソルバに論理式の否定を追加する．
  ///
  /// clit が true の時に与えられた論理式が false となる条件を追加する．
  /// 論理式の変数番号はノード番号に対応している．
  void
  add_negation(
    const Expr& expr, ///< [in] 対象の論理式
    SatLiteral clit   ///< [in] 制御用のリテラル
  );

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  virtual
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief SAT問題が充足可能か調べる．
  /// @return 結果を返す．
  SatBool3
  check(
    const vector<SatLiteral>& assumptions  ///< [in] 値の決まっている変数のリスト
  );

  /// @brief 直前の solve() の結果からテストベクタを作る．
  /// @return 作成したテストベクタを返す．
  ///
  /// この関数では単純に外部入力の値を記録する．
  TestVector
  get_tv();

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  ///
  /// * FFR内の故障伝搬条件は含まない．
  NodeValList
  get_sufficient_condition(
    const TpgNode* ffr_root ///< [in] FFRの根のノード
  );

  /// @brief 必要条件を取り出す．
  /// @return 必要条件を返す．
  NodeValList
  get_mandatory_condition(
    const NodeValList& ffr_cond, ///< [in] FFR内の伝搬条件
    const NodeValList& suf_cond	 ///< [in] 十分条件
  );

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const { return mSolver.get_stats(); }

  /// @brief SATの計算時間を返す．
  double
  sat_time() const { return mSatTime; }


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
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
    return network().node_num();
  }

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief 1時刻前の正常値の変数を返す．
  SatLiteral
  hvar(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    ASSERT_COND( mHvarMap(node) != SatLiteral::X );

    return mHvarMap(node);
  }

  /// @brief 正常値の変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    return mGvarMap(node);
  }

  /// @brief 故障値の変数を返す．
  SatLiteral
  fvar(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    return mFvarMap(node);
  }

  /// @brief 伝搬条件の変数を返す．
  SatLiteral
  dvar(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    return mDvarMap(node);
  }

  /// @brief 1時刻前の正常値の変数を設定する．
  void
  set_hvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var	 ///< [in] 設定する変数
  )
  {
    mHvarMap.set_vid(node, var);
  }

  /// @brief 正常値の変数を設定する．
  void
  set_gvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var	 ///< [in] 設定する変数
  )
  {
    mGvarMap.set_vid(node, var);
  }

  /// @brief 故障値値の変数を設定する．
  void
  set_fvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var	 ///< [in] 設定する変数
  )
  {
    mFvarMap.set_vid(node, var);
  }

  /// @brief 故障伝搬条件の変数を設定する．
  void
  set_dvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var	 ///< [in] 設定する変数
  )
  {
    mDvarMap.set_vid(node, var);
  }

  /// @brief 1時刻前の正常値の変数マップを返す．
  const VidMap&
  hvar_map() const
  {
    return mHvarMap;
  }

  /// @brief 正常値の変数マップを返す．
  const VidMap&
  gvar_map() const
  {
    return mGvarMap;
  }

  /// @brief 故障値の変数マップを返す．
  const VidMap&
  fvar_map() const
  {
    return mFvarMap;
  }

  /// @brief 1時刻前の正常値を得る．
  Val3
  hval(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    auto var = hvar(node);
    return get_val(var);
  }

  /// @brief 正常値を得る．
  Val3
  gval(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    auto var = gvar(node);
    return get_val(var);
  }

  /// @brief 故障値を得る．
  Val3
  fval(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    auto var = fvar(node);
    return get_val(var);
  }

  /// @brief 直前の solve() の解を返す．
  const SatModel&
  sat_model() const
  {
    return mSatModel;
  }

  /// @brief 起点となるノードを返す．
  const TpgNode*
  root_node() const
  {
    return mRoot;
  }

  /// @brief root_node() の TFO に含まれる出力のノードのリストを返す．
  const vector<const TpgNode*>&
  output_list() const
  {
    return mOutputList;
  }

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

  /// @brief ノード名を返す．
  string
  node_name(
    const TpgNode* node ///< [in] ノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_cnf() の追加処理
  ///
  /// デフォルトではなにもしない．
  virtual
  void
  opt_make_cnf();

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node  ///< [in] 対象のノード
  );

  /// @brief add_negation の下請け関数
  SatLiteral
  _add_negation_sub(
    const Expr& expr  ///< [in] 論理式
  );

  /// @brief SATモデルから値を取り出す．
  Val3
  get_val(
    SatLiteral var  ///< [in] 変数番号
  ) const
  {
    auto sat_val = mSatModel[var];
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot;

  // dchain を生成するとき true にするフラグ
  bool mDchain{false};

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // mTfiList に関連する DFF の入力のリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mTfi2List;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 関係する１時刻目の外部入力ノードを入れておくリスト
  vector<const TpgNode*> mAuxInputList;

  // 関係する擬似外部入力ノードを入れておくリスト
  vector<const TpgNode*> mPPIList;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // SATの解を保持する配列
  SatModel mSatModel;

  // SAT時間
  double mSatTime;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE2_H
