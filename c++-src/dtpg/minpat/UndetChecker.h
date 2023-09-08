#ifndef UNDETCHECKER_H
#define UNDETCHECKER_H

/// @file UndetChecker.h
/// @brief UndetChecker のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "GateEnc.h"

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/Timer.h"

#include "VidMap.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UndetChecker UndetChecker.h "UndetChecker.h"
/// @brief 支配関係の判定を行うクラス
//////////////////////////////////////////////////////////////////////
class UndetChecker
{
public:

  /// @brief コンストラクタ
  UndetChecker(
    const TpgNetwork& network,            ///< [in] 対象のネットワーク
    FaultType fault_type,                 ///< [in] 故障の種類
    const TpgFault* fault,                ///< [in] 故障伝搬をさせない故障
    const SatSolverType& solver_type = {} ///< [in] SATソルバの実装タイプ
  );

  /// @brief デストラクタ
  ~UndetChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @return 結果を返す．
  SatBool3
  check(
    const NodeValList& cond ///< [in] 条件
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
    NodeVal node_val ///< [in] 値割り当て
  );

  /// @brief 値割り当てをリテラルのリストに変換する．
  /// @retval true 正しく変換された．
  /// @retval false CNF節を作っていない部分の割り当てがあった．
  bool
  conv_to_assumptions(
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    vector<SatLiteral>& assumptions ///< [ou] 変換したリテラルを追加するリスト
  );

  /// @brief 一つの SAT問題を解く．
  /// @return 結果を返す．
  ///
  /// mSolver.solve() を呼び出すだけだが統計情報の更新を行っている．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions ///< [in] 値の決まっている変数のリスト
  );

  /// @brief 直前の solve() の解を返す．
  const SatModel&
  model();

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

  /// @brief 故障値の変数を返す．
  SatLiteral
  fvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap(node);
  }

  /// @brief gvar が割り当てられている時に true を返す．
  bool
  has_gvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    if ( (mMarkArray[node->id()] & 8U) != 0U ) {
      return true;
    }
    else {
      return false;
    }
  }

  /// @brief hvar が割り当てられている時に true を返す．
  bool
  has_hvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    if ( (mMarkArray[node->id()] & 16U) != 0U ) {
      return true;
    }
    else {
      return false;
    }
  }


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

  /// @brief 1時刻前の正常値の変数を設定する．
  void
  set_hvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var       ///< [in] 設定する変数
  )
  {
    mHvarMap.set_vid(node, var);
    mMarkArray[node->id()] |= 16U;
  }

  /// @brief 正常値の変数を設定する．
  void
  set_gvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var       ///< [in] 設定する変数
  )
  {
    mGvarMap.set_vid(node, var);
    mFvarMap.set_vid(node, var);
    mMarkArray[node->id()] |= 8U;
  }

  /// @brief 故障値値の変数を設定する．
  void
  set_fvar(
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral var       ///< [in] 設定する変数
  )
  {
    mFvarMap.set_vid(node, var);
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

  /// @brief TFO マークをつける．
  ///
  /// と同時に mTfoList に入れる．<br>
  /// 出力ノードの場合は mOutputList にも入れる．<br>
  /// すでにマークされていたら何もしない．
  void
  set_tfo_mark(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    int id = node->id();
    std::uint8_t mask = 1U;
    if ( (mMarkArray[id] & mask) == 0U ) {
      mMarkArray[id] |= mask;
      mTfoList.push_back(node);
      if ( node->is_ppo() ) {
	mOutputList.push_back(node);
      }
      set_tfi_mark(node);
    }
  }

  /// @brief TFI マークをつける．
  ///
  /// と同時に mTfiList に入れる．
  void
  set_tfi_mark(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    int id = node->id();
    std::uint8_t mask = 2U;
    if ( (mMarkArray[id] & mask) == 0U ) {
      mMarkArray[id] |= mask;
      mTfiList.push_back(node);
      if ( mFaultType == FaultType::TransitionDelay && node->is_dff_output() ) {
	mDffInputList.push_back(node->alt_node());
      }
    }
  }

  /// @brief prev TFI マークをつける．
  ///
  /// と同時に mPrevTfiList に入れる．
  void
  set_prev_tfi_mark(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    int id = node->id();
    std::uint8_t mask = 4U;
    if ( (mMarkArray[id] & mask) == 0U ) {
      mMarkArray[id] |= mask;
      mPrevTfiList.push_back(node);
    }
  }

  /// @brief 正常回路の CNF を作る．
  void
  make_good_cnf(
    const TpgNode* node ///< [in] 対象のノード
  );

  /// @brief 1時刻前の正常回路の CNF を作る．
  void
  make_prev_cnf(
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

  // 故障の種類
  FaultType mFaultType;

  // 対象の故障
  const TpgFault* mFault;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot;

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF の入力を入れておくリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mPrevTfiList;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  // bit-0: TFOマーク
  // bit-1: TFIマーク
  // bit-2: prev TFIマーク
  // bit-3: gvar マーク
  // bit-4: hvar マーク
  vector<std::uint8_t> mMarkArray;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 正常回路用の符号器
  GateEnc mGvalEnc;

  // 1時刻前の正常回路用の符号器
  GateEnc mHvalEnc;

  // 故障回路用の符号器
  GateEnc mFvalEnc;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // UNDETCHECKER_H
