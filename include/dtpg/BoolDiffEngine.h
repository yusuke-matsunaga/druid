#ifndef BOOLDIFFENGINE_H
#define BOOLDIFFENGINE_H

/// @file BoolDiffEngine.h
/// @brief BoolDiffEngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgStats.h"
#include "Val3.h"
#include "VidMap.h"
#include "NodeValList.h"
#include "Justifier.h"

#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BoolDiffEngine BoolDiffEngine.h "BoolDiffEngine.h"
/// @brief ブール微分を行うクラス
///
/// network 上で node の値の反転がいずれかの外部出力まで伝搬する
/// 条件を表す CNF を生成する．
/// いわゆる「ブール微分」を行っている．
///
/// network で仮定されている故障タイプが遷移故障の場合，１時刻前の
/// 値を表す CNF も生成される．
///
/// option の仕様は以下の通り
/// - "sat_param": JSONオブジェクト
///                SATソルバの初期化パラメータ
//////////////////////////////////////////////////////////////////////
class BoolDiffEngine
{
public:

  /// @brief コンストラクタ
  ///
  /// このオブジェクトが生成された時点で root_node の値の反転が
  /// いずれかの外部出力まで伝搬する条件を表す CNF が生成されている．
  BoolDiffEngine(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* root_node,	   ///< [in] 起点となるノード
    const JsonValue& option        ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~BoolDiffEngine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // ネットワークの構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief ノード番号の最大値を返す．
  SizeType
  max_node_id() const
  {
    return network().node_num();
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
  cur_node_list() const
  {
    return mTfiList;
  }

  /// @brief 関係する１時刻前のノードのリストを返す．
  const vector<const TpgNode*>&
  prev_node_list() const
  {
    return mTfi2List;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // SATソルバ関係の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

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
  new_variable(
    bool decision = true
  )
  {
    return solver().new_variable(decision);
  }

  /// @brief SATソルバに節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] 節を構成するリテラルのリスト
  )
  {
    solver().add_clause(lits);
  }

  /// @brief 制約条件のもとでSAT問題を解く．
  SatBool3
  check(
    const vector<SatLiteral>& assumptions ///< [in] 制約条件
  )
  {
    return mSolver.solve(assumptions);
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  NodeValList
  extract_sufficient_condition();

  /// @brief 与えられた割当の正当化を行う．
  /// @return 外部入力の値割り当てを返す．
  ///
  /// この関数内では SAt ソルバを起動しない．
  /// assign_list の割当は直前の check() の結果に沿ったものになっている
  /// 必要がある．
  NodeValList
  justify(
    const NodeValList& assign_list ///< [in] もととなる値割り当て
  );

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

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const { return mCnfTime; }

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const { return mSolver.get_stats(); }


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF の生成を行う．
  ///
  /// node からいずれかの外部出力へ故障の影響が伝搬する
  /// 条件を表す CNF を作る．
  void
  make_cnf();

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
  void
  make_dchain_cnf(
    const TpgNode* node  ///< [in] 対象のノード
  );

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot;

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

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // CNFの生成時間
  double mCnfTime;

  // extractor 用のオプションパラメータ
  JsonValue mExOption;

  // justifier
  Justifier mJustifier;

};

END_NAMESPACE_DRUID

#endif // BOOLDIFFENGINE_H
