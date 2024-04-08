#ifndef DTPGENGINE_H
#define DTPGENGINE_H

/// @file DtpgEngine.h
/// @brief DtpgEngine のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "Val3.h"
#include "VidMap.h"
#include "NodeValList.h"
#include "Justifier.h"

#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine DtpgEngine.h "DtpgEngine.h"
/// @brief DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgEngine(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* root,	   ///< [in] 故障伝搬の起点となるノード
    bool make_dchain,              ///< [in] dchain を作る時 true にする．
    const string& ex_mode,         ///< [in] extractor のモード
    const string& just_mode,       ///< [in] justifier のモード
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  );

  /// @brief デストラクタ
  ~DtpgEngine() = default;


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
    return solver().new_variable(true);
  }

  /// @brief SATソルバに節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] 節を構成するリテラルのリスト
  )
  {
    solver().add_clause(lits);
  }

  /// @brief テストパタン生成を行う．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 故障
  );

  /// @brief solve() が成功した時にテストパタンを生成する．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 故障
  )
  {
    auto assign_list = get_sufficient_condition(fault);
    return justify(assign_list);
  }

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  NodeValList
  get_sufficient_condition(
    const TpgFault* fault ///< [in] 故障
  );

  /// @brief 十分条件からテストベクタを作る．
  TestVector
  justify(
    const NodeValList& assign_list ///< [in] 十分条件
  );

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const { return mSolver.get_stats(); }


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

  /// @brief 対象の部分回路の関係を表す変数を用意する．
  void
  prepare_vars();

  /// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
  void
  gen_good_cnf();

  /// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
  void
  gen_faulty_cnf();


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

  /// @brief gen_pattern() で用いる検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  virtual
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node  ///< [in] 対象のノード
  );


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

  // dchain を生成するとき true にするフラグ
  bool mDchain{false};

  // extractor のモード
  string mExMode;

  // justifier
  Justifier mJustifier;

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

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE_H
