#ifndef DOMCHECKER_H
#define DOMCHECKER_H

/// @file DomChecker.h
/// @brief DomChecker のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/Timer.h"

#include "VidMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomChecker DomChecker.h "DomChecker.h"
/// @brief 支配関係の判定を行うクラス
///
/// f1 が検出可能で f2 が検出不可能なパタンが存在しないことを調べるための
/// クラス．
/// f1 の属する FFR の根のノードを root で指定する．
/// f2 は fault で指定する．
//////////////////////////////////////////////////////////////////////
class DomChecker
{
public:

  /// @brief コンストラクタ
  DomChecker(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* root,	   ///< [in] 故障伝搬の起点となるノード
    const TpgFault* fault,	   ///< [in] 故障伝搬をさせない故障
    const SatInitParam& init_param ///< [in] SATソルバの実装タイプ
    = SatInitParam{}
  );

  /// @brief デストラクタ
  ~DomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// fault の結果が FFR の根まで伝搬する条件のもとで
  /// SAT かどうかを調べる．
  /// UNSAT の場合には支配故障であることがわかる．
  SatBool3
  check(
    const TpgFault* fault ///< [in] 対象の故障
  );

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const
  {
    return mStats;
  }

  /// @brief 値割り当てをリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeTimeVal node_val
  );

  /// @brief 値割り当てをリテラルのリストに変換する．
  void
  conv_to_assumptions(
    const NodeTimeValList& assign_list, ///< [in] 値の割り当てリスト
    vector<SatLiteral>& assumptions     ///< [out] 変換したリテラルを追加するリスト
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
    const vector<SatLiteral>& lits
  )
  {
    solver().add_clause(lits);
  }

  /// @brief 一つの SAT問題を解く．
  /// @return 結果を返す．
  ///
  /// mSolver.solve() を呼び出すだけだが統計情報の更新を行っている．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions ///< [in] 値の決まっている変数のリスト
  );


protected:
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
  double
  timer_stop();

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief 1時刻前の正常値の変数を返す．
  SatLiteral
  hvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    ASSERT_COND( mHvarMap(node) != SatLiteral::X );

    return mHvarMap(node);
  }

  /// @brief 正常値の変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mGvarMap(node);
  }

  /// @brief 故障1の故障値の変数を返す．
  SatLiteral
  fvar1(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap1(node);
  }

  /// @brief 故障2の故障値の変数を返す．
  SatLiteral
  fvar2(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap2(node);
  }

  /// @brief 故障1の伝搬条件の変数を返す．
  SatLiteral
  dvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mDvarMap(node);
  }

  /// @brief 1時刻前の正常値の変数を設定する．
  void
  set_hvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var       ///< [in] 設定する変数
  )
  {
    mHvarMap.set_vid(node, var);
  }

  /// @brief 正常値の変数を設定する．
  void
  set_gvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var       ///< [in] 設定する変数
  )
  {
    mGvarMap.set_vid(node, var);
  }

  /// @brief 故障1の故障値値の変数を設定する．
  void
  set_fvar1(
    const TpgNode* node,  ///< [in] 対象のノード
    SatLiteral var        ///< [in] 設定する変数
  )
  {
    mFvarMap1.set_vid(node, var);
  }

  /// @brief 故障2の故障値値の変数を設定する．
  void
  set_fvar2(
    const TpgNode* node,  ///< [in] 対象のノード
    SatLiteral var        ///< [in] 設定する変数
  )
  {
    mFvarMap2.set_vid(node, var);
  }

  /// @brief 故障伝搬条件の変数を設定する．
  void
  set_dvar(
    const TpgNode* node,  ///< [in] 対象のノード
    SatLiteral var        ///< [in] 設定する変数
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

  /// @brief 故障1の故障値の変数マップを返す．
  const VidMap&
  fvar1_map() const
  {
    return mFvarMap1;
  }

  /// @brief 故障2の故障値の変数マップを返す．
  const VidMap&
  fvar2_map() const
  {
    return mFvarMap2;
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
  gen_faulty_cnf1();

  /// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
  void
  gen_faulty_cnf2();

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
  void
  make_dchain_cnf(
    const TpgNode* node ///< [in] 対象のノード
  );


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

  // 故障2
  const TpgFault* mFault2;

  // 故障1の故障伝搬の起点となるノード
  const TpgNode* mRoot1;

  // 故障2の故障伝搬の起点となるノード
  const TpgNode* mRoot2;

  // 故障1のTFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList1;

  // 故障2のTFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList2;

  // 故障1に関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList1;

  // 故障2に関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList2;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF の入力を入れておくリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mPrevTfiList;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障1の故障値を表す変数のマップ
  VidMap mFvarMap1;

  // 故障2の故障値を表す変数のマップ
  VidMap mFvarMap2;

  // 故障1の故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // DOMCHECKER_H
