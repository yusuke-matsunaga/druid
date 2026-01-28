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
#include "types/TpgNodeList.h"
#include "types/AssignList.h"
#include "ym/SatSolver.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

class SubEnc;
class Justifier;

//////////////////////////////////////////////////////////////////////
/// @class StructEngine StructEngine.h "StructEngine.h"
/// @brief DTPG 用の基本的なエンコードを行うクラス
/// @ingroup DtpgGroup
///
/// SATソルバとノード用の変数マップを持つ．
/// 具体的な仕事は SubEnc の継承クラスを登録することで行う．
//////////////////////////////////////////////////////////////////////
class StructEngine
{
public:

  /// @brief コンストラクタ
  StructEngine(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] 初期化オプション
    = JsonValue{}              ///<      "sat_param": JsonValue
                               ///<      SATソルバの初期化パラメータ
                               ///<      "justifier": string
                               ///<      Justifier の初期化パラメータ
  );

  /// @brief デストラクタ
  ///
  /// 保持している SubEnc はここで開放される．
  ~StructEngine();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SubEnc を追加する．
  void
  add_subenc(
    std::unique_ptr<SubEnc>&& enc
  );

  /// @brief 現時刻で考慮するノードを追加する．
  ///
  /// 実際にはこのノードのTFOのTFIを対象にする．
  void
  add_cur_node(
    const TpgNode& node
  );

  /// @brief 現時刻で考慮するノードのリストを追加する．
  ///
  /// 実際にはこのノードのTFOのTFIを対象にする．
  void
  add_cur_node_list(
    const TpgNodeList& node_list
  )
  {
    for ( auto node: node_list ) {
      add_cur_node(node);
    }
  }

  /// @brief 1時刻前で考慮するノードを追加する．
  ///
  /// 実際にはこのノードのTFIを対象にする．
  void
  add_prev_node(
    const TpgNode& node
  );

  /// @brief 1時刻前で考慮するノードのリストを追加する．
  ///
  /// 実際にはこのノードのTFIを対象にする．
  void
  add_prev_node_list(
    const TpgNodeList& node_list
  )
  {
    for ( auto node: node_list ) {
      add_prev_node(node);
    }
  }

  /// @brief 未処理のノードを処理する．
  void
  update()
  {
    if ( mState != DIRTY ) {
      return;
    }
    if ( mCurNodeCandList.empty() &&
	 mPrevNodeCandList.empty() &&
	 mSubEncCandList.empty() ) {
      return;
    }
    _update();
  }

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
  std::vector<SatLiteral>
  conv_to_literal_list(
    const AssignList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 与えられた論理式を充足させるCNF式を作る．
  /// @return 条件を表すリテラルのリストを返す．
  ///
  /// 論理式中の変数番号は TpgNode->id() * 2 + time に対応している．
  std::vector<SatLiteral>
  expr_to_cnf(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 対象のネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 関連するノードのリスト
  const TpgNodeList&
  cur_node_list() const
  {
    return mCurNodeList;
  }

  /// @brief 1時刻前の値に関連するノードのリスト
  const TpgNodeList&
  prev_node_list() const
  {
    return mPrevNodeList;
  }

  /// @brief 変数を作る．
  SatLiteral
  new_variable(
    bool decision = false ///< [in] 決定変数のときに true とする．
  );

  /// @brief SAT問題を解く
  SatBool3
  solve(
    const std::vector<SatLiteral>& assumptions ///< [in] アサートするリテラルのリスト
    = {}
  );

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const;

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    update();
    return mSolver;
  }

  /// @brief ノードの値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode& node ///< [in] 対象のノード
  )
  {
    update();
    return mGvarMap(node);
  }

  /// @brief ノードの値を表す変数の辞書を返す．
  const VidMap&
  gvar_map()
  {
    update();
    return mGvarMap;
  }

  /// @brief ノードの1時刻前の値を表す変数を返す．
  SatLiteral
  hvar(
    const TpgNode& node ///< [in] 対象のノード
  )
  {
    update();
    return mHvarMap(node);
  }

  /// @brief 値を返す．
  ///
  /// 直前に solver().solve() を呼んでいる必要がある．
  bool
  val(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻(0 or 1)
  );

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

  /// @brief 回路の構造を表すCNFを生成する．
  void
  _update();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 部品のリスト
  std::vector<std::unique_ptr<SubEnc>> mSubEncList;

  // 現時刻に関係のあるノードのリスト
  TpgNodeList mCurNodeList;

  // 1時刻前に関係のあるノードのリスト
  TpgNodeList mPrevNodeList;

  // 未処理のノードが残っていることを示すフラグ
  enum State {
    STABLE,
    UPDATING,
    DIRTY
  };
  State mState{STABLE};

  // make_cnf() を呼んでいない SubEnc のリスト
  std::vector<SubEnc*> mSubEncCandList;

  // mCurNodeList に追加する予定のノードリスト
  TpgNodeList mCurNodeCandList;

  // mPrevNodeList に追加する予定のノードリスト
  TpgNodeList mPrevNodeCandList;

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
  SubEnc() = default;

  /// @brief デストラクタ
  virtual
  ~SubEnc() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 親の StructEngine を返す．
  StructEngine&
  engine() const
  {
    return *mEngine;
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

  /// @brief 変数を作る．
  SatLiteral
  new_variable(
    bool decision = false ///< [in] 決定変数のときに true とする．
  )
  {
    return engine().new_variable(decision);
  }

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  std::vector<SatLiteral>
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

  /// @brief データ構造の初期化を行う．
  virtual
  void
  init() = 0;

  /// @brief 変数を割り当てCNFを生成する．
  virtual
  void
  make_cnf() = 0;

  /// @brief 関連するノードのリストを返す．
  virtual
  const TpgNodeList&
  node_list() const;

  /// @brief 1時刻前の値に関連するノードのリストを返す．
  virtual
  const TpgNodeList&
  prev_node_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の StructEngine
  StructEngine* mEngine{nullptr};

};

END_NAMESPACE_DRUID

#endif // STRUCTENGINE_H
