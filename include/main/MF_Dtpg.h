#ifndef MF_DTPG_H
#define MF_DTPG_H

/// @file MF_Dtpg.h
/// @brief MF_Dtpg のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgResult.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "Justifier.h"
#include "VidMap.h"

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MF_Dtpg MF_Dtpg.h "MF_Dtpg.h"
/// @brief 多重故障用の DTPG
//////////////////////////////////////////////////////////////////////
class MF_Dtpg
{
public:

  /// @brief コンストラクタ
  MF_Dtpg(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const string& just_type,	     ///< [in] Justifier の種類を表す文字列
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType{}
  );

  /// @brief デストラクタ
  ~MF_Dtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(
    const vector<const TpgFault*>& fault_list  ///< [in] 対象の多重故障
  );

  /// @brief 統計情報を得る．
  const DtpgStats&
  stats() const
  {
    return mStats;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  NodeValList
  get_sufficient_condition();

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
  /// SATだった場合のモデルは mSatModel に格納される．
  DtpgResult
  solve(
    const vector<SatLiteral>& assumptions  ///< [in] 値の決まっている変数のリスト
  );

  /// @brief SAT問題が充足可能か調べる．
  /// @return 結果を返す．
  ///
  /// solve() との違いは結果のモデルを保持しない．
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

  /// @brief バックトレースを行う．
  /// @return テストパタンを返す．
  TestVector
  backtrace(
    const NodeValList& suf_cond  ///< [in] 十分条件の割り当て
  );

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
    SatLiteral var = hvar(node);
    return get_val(var);
  }

  /// @brief 正常値を得る．
  Val3
  gval(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    SatLiteral var = gvar(node);
    return get_val(var);
  }

  /// @brief 故障値を得る．
  Val3
  fval(
    const TpgNode* node  ///< [in] 対象のノード
  ) const
  {
    SatLiteral var = fvar(node);
    return get_val(var);
  }

  /// @brief 直前の solve() の解を返す．
  const SatModel&
  sat_model() const
  {
    return mSatModel;
  }

  /// @brief 起点となるノードのリストを返す．
  const vector<const TpgNode*>&
  root_node_list() const
  {
    return mRootList;
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

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node  ///< [in] 対象のノード
  );

  /// @brief TFO マークをつける．
  ///
  /// と同時に mTfoList に入れる．<br>
  /// 出力ノードの場合は mOutputList にも入れる．<br>
  /// すでにマークされていたら何もしない．
  void
  set_tfo_mark(
    const TpgNode* node  ///< [in] 対象のノード
  )
  {
    SizeType id = node->id();
    if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
      mMarkArray[id] |= 1U;
      mTfoList.push_back(node);
      if ( node->is_ppo() ) {
	mOutputList.push_back(node);
      }
      if ( mFaultType == FaultType::TransitionDelay ) {
	if ( node->is_primary_input() ) {
	  mAuxInputList.push_back(node);
	}
      }
      else {
	if ( node->is_ppi() ) {
	  mPPIList.push_back(node);
	}
      }
    }
  }

  /// @brief TFI マークをつける．
  ///
  /// と同時に mTfiList に入れる．
  void
  set_tfi_mark(
    const TpgNode* node  ///< [in] 対象のノード
  )
  {
    SizeType id = node->id();
    if ( (mMarkArray[id] & 3U) == 0U ) {
      mMarkArray[id] |= 2U;
      mTfiList.push_back(node);
      if ( mFaultType == FaultType::TransitionDelay ) {
	if ( node->is_dff_output() ) {
	  mDffInputList.push_back(node->alt_node());
	}
	else if ( node->is_primary_input() ) {
	  mAuxInputList.push_back(node);
	}
      }
      else {
	if ( node->is_ppi() ) {
	  mPPIList.push_back(node);
	}
      }
    }
  }

  /// @brief TFI2 マークをつける．
  ///
  /// と同時に mTfi2List に入れる．
  void
  set_tfi2_mark(
    const TpgNode* node  ///< [in] 対象のノード
  )
  {
    SizeType id = node->id();
    if ( ((mMarkArray[id] >> 2) & 1U) == 0U ) {
      mMarkArray[id] |= 4U;
      mTfi2List.push_back(node);
      if ( node->is_ppi() ) {
	mPPIList.push_back(node);
      }
    }
  }

  /// @brief root マークをつける．
  void
  set_root_mark(
    const TpgNode* node  ///< [in] 対象のノード
  )
  {
    SizeType id = node->id();
    mMarkArray[id] |= 8U;
  }

  /// @brief root マークを読む．
  bool
  root_mark(
    const TpgNode* node  ///< [in] 対象のノード
  )
  {
    SizeType id = node->id();
    return static_cast<bool>((mMarkArray[id] >> 3) & 1U);
  }

  /// @brief SATモデルから値を取り出す．
  Val3
  get_val(
    SatLiteral var  ///< [in] 変数番号
  ) const
  {
    SatBool3 sat_val = mSatModel[var];
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

  // 統計情報
  DtpgStats mStats;

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 故障伝搬の起点となるノードのリスト
  vector<const TpgNode*> mRootList;

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF の入力を入れておくリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mTfi2List;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 関係する１時刻目の外部入力ノードを入れておくリスト
  vector<const TpgNode*> mAuxInputList;

  // 関係する擬似外部入力ノードを入れておくリスト
  vector<const TpgNode*> mPPIList;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<std::uint8_t> mMarkArray;

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

  // バックトレーサー
  Justifier mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  Timer mTimer;

};

END_NAMESPACE_DRUID

#endif // MF_DTPG_H
