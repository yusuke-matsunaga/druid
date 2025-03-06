#ifndef STRUCTENGINE_H
#define STRUCTENGINE_H

/// @file StructEngine.h
/// @brief StructEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "VidMap.h"
#include "AssignList.h"
#include "ym/SatSolver.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

class SubEnc;
class Justifier;

//////////////////////////////////////////////////////////////////////
/// @class StructEngine StructEngine.h "StructEngine.h"
/// @brief DTPG 用の基本的なエンコードを行うクラス
///
/// SATソルバとノード用の変数マップを持つ．
/// 具体的な仕事は SubEnc の継承クラスを登録することで行う．
///
/// make_cnf() などの関数
//////////////////////////////////////////////////////////////////////
class StructEngine
{
  friend class SubEnc;

public:

  /// @brief コンストラクタ
  explicit
  StructEngine(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] 初期化オプション
    = JsonValue{}              ///<      "sat_param": JsonValue
                               ///<                   SATソルバの初期化パラメータ
                               ///<      "justifier": string
                               ///<                   Justifier の初期化パラメータ
  );

  /// @brief デストラクタ
  ///
  /// 保持している SubEnc はここで開放される．
  ~StructEngine();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 回路の構造を表すCNFを生成する．
  void
  make_cnf(
    const vector<const TpgNode*>& cur_node_list, ///< [in] 関係するノードのリスト
    const vector<const TpgNode*>& prev_node_list ///< [in] 1時刻前の値に関係するノードのリスト
  );

  /// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
  /// @return 外部入力の割り当てリストを返す．
  ///
  /// * 事前にSAT問題の充足解が求められている必要がある．
  /// * 必要な値割り当てのみが記録される．
  AssignList
  justify(
    const AssignList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 現在の外部入力の割当を得る．
  /// @return 外部入力の割り当てリストを返す．
  ///
  /// * 事前にSAT問題の充足解が求められている必要がある．
  /// * すべての外部入力になんらかの値が入る
  AssignList
  get_pi_assign();

  /// @brief 値割り当てを対応するリテラルに変換する．
  SatLiteral
  conv_to_literal(
    Assign assign ///< [in] 値割り当て
  );

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    const AssignList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 与えられた論理式を充足させるCNF式を作る．
  /// @return 条件を表すリテラルのリストを返す．
  ///
  /// 論理式中の変数番号は TpgNode->id() * 2 + time に対応している．
  vector<SatLiteral>
  make_cnf(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 対象のネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 関連するノードのリスト
  const vector<const TpgNode*>&
  cur_node_list() const
  {
    return mCurNodeList;
  }

  /// @brief 1時刻前の値に関連するノードのリスト
  const vector<const TpgNode*>&
  prev_node_list() const
  {
    return mPrevNodeList;
  }

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief SATソルバを返す．
  const SatSolver&
  solver() const
  {
    return mSolver;
  }

  /// @brief ノードの値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mGvarMap(node);
  }

  /// @brief ノードの値を表す変数の辞書を返す．
  const VidMap&
  gvar_map() const
  {
    return mGvarMap;
  }

  /// @brief ノードの1時刻前の値を表す変数を返す．
  SatLiteral
  hvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mHvarMap(node);
  }

  /// @brief 値を返す．
  ///
  /// 直前に solver().solve() を呼んでいる必要がある．
  bool
  val(
    const TpgNode* node, ///< [in] 対象のノード
    int time             ///< [in] 時刻(0 or 1)
  ) const;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const
  {
    return mCnfTime;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 部品を登録する．
  ///
  /// subenc の所有権は StructEngine に委譲される．
  void
  reg_subenc(
    SubEnc* subenc ///< [in] 部品のエンコーダ
  )
  {
    mSubEncList.push_back(subenc);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 部品のリスト
  vector<SubEnc*> mSubEncList;

  // 現時刻に関係のあるノードのリスト
  vector<const TpgNode*> mCurNodeList;

  // DFF の入力ノードのリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前に関係のあるノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // SATソルバ
  SatSolver mSolver;

  // ノード用の変数マップ
  VidMap mGvarMap;

  // 1時刻前のノード用の変数マップ
  VidMap mHvarMap;

  // justifier
  std::unique_ptr<Justifier> mJustifier;

  // 時間計測用のタイマ
  Timer mTimer;

  // CNF生成時間
  double mCnfTime;

};


//////////////////////////////////////////////////////////////////////
/// @class SubEnc SubEnc.h "SubEnc.h"
/// @brief StructEngine の部品クラス
///
/// このクラスは実際の継承クラスの純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class SubEnc
{
  friend class StructEngine;

public:

  /// @brief コンストラクタ
  SubEnc(
    StructEngine& engine ///< [in] 親の StructEngine
  ) : mEngine{engine}
  {
    mEngine.reg_subenc(this);
  }

  /// @brief デストラクタ
  virtual
  ~SubEnc() {}


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 親の StructEngine を返す．
  StructEngine&
  engine() const
  {
    return mEngine;
  }

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return engine().network();
  }

  /// @brief SATソルバーを返す．
  SatSolver&
  solver()
  {
    return engine().solver();
  }

  /// @brief 値割り当てを対応するリテラルに変換する．
  SatLiteral
  conv_to_literal(
    Assign assign ///< [in] 値割り当て
  )
  {
    return engine().conv_to_literal(assign);
  }

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    AssignList& assign_list ///< [in] 値割り当てのリスト
  )
  {
    return engine().conv_to_literal_list(assign_list);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // StructEngine から利用される仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を割り当てCNFを生成する．
  virtual
  void
  make_cnf() = 0;

  /// @brief 関連するノードのリストを返す．
  virtual
  const vector<const TpgNode*>&
  node_list() const;

  /// @brief 1時刻前の値に関連するノードのリストを返す．
  virtual
  const vector<const TpgNode*>&
  prev_node_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の StructEngine
  StructEngine& mEngine;

};

END_NAMESPACE_DRUID

#endif // STRUCTENGINE_H
