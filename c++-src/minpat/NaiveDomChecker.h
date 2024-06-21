#ifndef NAIVEDOMCHECKER_H
#define NAIVEDOMCHECKER_H

/// @file NaiveDomChecker.h
/// @brief NaiveDomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "VidMap.h"
#include "NodeTimeValList.h"
#include "ym/SatBool3.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveDomChecker NaiveDomChecker.h "NaiveDomChecker.h"
/// @brief ナイーブな DomChecker
//////////////////////////////////////////////////////////////////////
class NaiveDomChecker
{
public:

  /// @brief コンストラクタ
  NaiveDomChecker(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgFault* fault1,        ///< [in] 故障1
    const TpgFault* fault2,        ///< [in] 故障2
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
    = SatInitParam{}
  );

  /// @brief デストラクタ
  ~NaiveDomChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief チェックする．
  /// @return 結果を返す．
  ///
  /// fault1 を検出して fault2 を検出しないテストパタンがあるか調べる．
  /// UNSAT の場合には fault1 が fault2 を支配している．
  bool
  check();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node ///< [in] 対象のノード
  );

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

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
    const vector<SatLiteral>& lits
  )
  {
    solver().add_clause(lits);
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

  /// @brief 故障1用の故障値の変数を返す．
  SatLiteral
  fvar1(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap1(node);
  }

  /// @brief 故障2用の故障値の変数を返す．
  SatLiteral
  fvar2(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap2(node);
  }

  /// @brief 伝搬条件の変数を返す．
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

  /// @brief 故障1用の故障値値の変数を設定する．
  void
  set_fvar1(
    const TpgNode* node,  ///< [in] 対象のノード
    SatLiteral var        ///< [in] 設定する変数
  )
  {
    mFvarMap1.set_vid(node, var);
  }

  /// @brief 故障2用の故障値値の変数を設定する．
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

  /// @brief 故障1用の故障値の変数マップを返す．
  const VidMap&
  fvar_map1() const
  {
    return mFvarMap1;
  }

  /// @brief 値割り当てをリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeTimeVal node_val
  )
  {
    auto node = node_val.node();
    bool inv = !node_val.val(); // 0 の時が inv = true
    auto vid = (node_val.time() == 0) ? hvar(node) : gvar(node);
    if ( inv ) {
      vid = ~vid;
    }
    return vid;
  }

  /// @brief 値割り当てをリテラルに変換する．
  vector<SatLiteral>
  conv_to_literals(
    const NodeTimeValList& nv_list
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障1
  const TpgFault* mFault1;

  // 故障2
  const TpgFault* mFault2;

  // 故障1の起点となるノード
  const TpgNode* mRoot1;

  // 故障2の起点となるノード
  const TpgNode* mRoot2;

  // 故障1のTFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList1;

  // 故障2のTFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList2;

  // いずれかのTFOノードのTFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF の入力を入れておくリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mPrevTfiList;

  // 故障1に関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList1;

  // 故障2に関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList2;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<std::uint8_t> mMarkArray;

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

};

END_NAMESPACE_DRUID

#endif // NAIVEDOMCHECKER_H
