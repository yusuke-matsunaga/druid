#ifndef STRUCTENC_H
#define STRUCTENC_H

/// @file StructEnc.h
/// @brief StructEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"
#include "VidMap.h"
#include "NodeValList.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

class PropCone;

//////////////////////////////////////////////////////////////////////
/// @class StructEnc StructEnc.h "StructEnc.h"
/// @brief TpgNetwork の構造に基づく SAT ソルバ
///
/// おおまかには SAT ソルバにネットワーク中のノードに対する変数
/// リテラルの割り当てを記録した辞書を加えたもの．
/// ノードに対する値の割り当てを与えると内部ではリテラルに変換される．
//////////////////////////////////////////////////////////////////////
class StructEnc
{
public:

  /// @brief コンストラクタ
  StructEnc(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~StructEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバに対するアクセス関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief ノード番号の最大値を返す．
  SizeType
  max_node_id() const
  {
    return mMaxId;
  }

  /// @brief 変数マップを得る．
  const VidMap&
  gvar_map() const
  {
    return mGvarMap;
  }

  /// @brief 変数マップを得る．
  const VidMap&
  hvar_map() const
  {
    return mHvarMap;
  }

  /// @brief 変数リテラルを得る．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] ノード
  ) const
  {
    return mGvarMap(node);
  }

  /// @brief 変数リテラルを得る．
  SatLiteral
  hvar(
    const TpgNode* node ///< [in] ノード
  ) const
  {
    return mHvarMap(node);
  }

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const { return mSolver.get_stats(); }


public:
  //////////////////////////////////////////////////////////////////////
  // 節を作る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief fault cone を追加する．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  void
  add_simple_cone(
    const TpgNode* fnode, ///< [in] 故障のあるノード
    bool detect		  ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief MFFC cone を追加する．
  ///
  /// fnode から到達可能な外部出力までの故障伝搬条件を考える．
  void
  add_mffc_cone(
    const TpgMFFC* mffc, ///< [in] MFFC の情報
    bool detect		 ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief 故障の伝搬条件を求める．
  vector<SatLiteral>
  make_prop_condition(
    const TpgNode* ffr_root ///< [in] FFR の根のノード
  );

  /// @brief 割当リストの内容を節に加える．
  ///
  /// すべて unit clause になる．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_assignments(
    const NodeValList& assignments ///< [in] 割当リスト
  );

  /// @brief 割当リストの否定の節を加える．
  ///
  /// リテラル数が assignment.size() の1つの節を加える．
  /// 必要に応じて使われているリテラルに関するCNFを追加する．
  void
  add_negation(
    const NodeValList& assignments ///< [in] 割当リスト
  );

  /// @brief 割当リストを仮定のリテラルに変換する．
  ///
  vector<SatLiteral>
  conv_to_literal_list(
    const NodeValList& assign_list ///< [in]  割当リスト
  );

  /// @brief 関係あるノードの入出力の関係を表すCNFを作る．
  void
  make_cnf();


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

  /// @brief 割当リストのもとでチェックを行う．
  SatBool3
  check_sat(
    const vector<SatLiteral>& assumptions, ///< [in] 割当リスト1
    const NodeValList& assign_list         ///< [in] 割当リスト2
  );

  /// @brief 伝搬条件を求める．
  ///
  /// check_sat() の実行結果が SatBool3::True の時のみ意味がある．
  NodeValList
  extract_prop_condition(
    const TpgNode* ffr_root ///< [in] FFR の根のノード
  );


public:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief デバッグ用のフラグをセットする．
  void
  set_debug(
    std::uint32_t bits
  )
  {
    mDebugFlag = bits;
  }

  /// @brief デバッグ用のフラグを得る．
  std::uint32_t
  debug() const
  {
    return mDebugFlag;
  }

  /// @brief ノード名を返す．
  string
  node_name(
    const TpgNode* node
  )
  {
    return node->str();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 関係あるノードに変数を割り当てる．
  void
  make_vars();

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
    if ( time == 1) {
      return gvar(node) * inv;
    }
    else {
      return hvar(node) * inv;
    }
  }

  /// @brief ノードに変数を割り当てる．
  void
  set_gvar(
    const TpgNode* node, ///< [in] ノード
    SatLiteral var	 ///< [in] 変数リテラル
  )
  {
    mGvarMap.set_vid(node, var);
  }

  /// @brief ノードに変数を割り当てる．
  void
  set_hvar(
    const TpgNode* node, ///< [in] ノード
    SatLiteral var	 ///< [in] 変数リテラル
  )
  {
    mHvarMap.set_vid(node, var);
  }

  /// @brief mCurNodeList を求める時に呼ばれるフック関数
  void
  tfi_hook(
    const TpgNode* node ///< [in] ノード
  )
  {
    if ( node->is_dff_output() ) {
      mDffInputList.push_back(node->alt_node());
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 1時刻前のの回路を持つ時 true にするフラグ
  bool mHasPrevState;

  // SAT ソルバ
  SatSolver mSolver;

  // ノード番号の最大値
  SizeType mMaxId;

  // 関係するノードのリスト
  vector<const TpgNode*> mCurNodeList;

  // mCurNodeList に関係する DFF の入力のリスト
  vector<const TpgNode*> mDffInputList;

  // 関係する１時刻前のノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // 変数マップ
  VidMap mGvarMap;

  // 変数マップ
  VidMap mHvarMap;

  // propagation cone のリスト
  vector<unique_ptr<PropCone>> mConeList;

  // FFR の根のノード番号をキーにして Cone 番号を格納する辞書
  unordered_map<SizeType, SizeType> mConeDict;

  // デバッグ用のフラグ
  std::uint32_t mDebugFlag;

};

END_NAMESPACE_DRUID

#endif // STRUCTENC_H
