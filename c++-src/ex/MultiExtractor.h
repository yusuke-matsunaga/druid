#ifndef MULTIEXTRACTOR_H
#define MULTIEXTRACTOR_H

/// @file MultiExtractor.h
/// @brief MultiExtractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "NodeValList.h"
#include "VidMap.h"
#include "Val3.h"
#include "ym/Expr.h"
#include "ym/HashSet.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MultiExtractor MultiExtractor.h "MultiExtractor.h"
/// @brief 十分割当を求めるクラス
///
/// 起点となるノードから外部出力まで故障の影響が伝搬する条件を求める．
/// その際に，正常回路の値だけで伝搬が保障される十分な割り当てとなる
/// ようにする．
///
/// ここでの割り当ては
/// - どの外部出力を選ぶのか
/// - 制御値で故障差の伝搬をマスクしている場合に複数のファンインが制御値
///   を持っていた場合にどれを選ぶのか
/// という選択を全て求める．
//////////////////////////////////////////////////////////////////////
class MultiExtractor
{
public:

  /// @brief コンストラクタ
  /// @param[in] gvar_map 正常値の変数番号のマップ
  /// @param[in] fvar_map 故障値の変数番号のマップ
  /// @param[in] model SATソルバの作ったモデル
  MultiExtractor(const VidMap& gvar_map,
		 const VidMap& fvar_map,
		 const vector<SatBool3>& model);

  /// @brief デストラクタ
  ~MultiExtractor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 各出力へ故障伝搬する値割り当てを求める．
  /// @param[in] root 起点となるノード
  /// @return 複数の値割り当てを表す論理式を返す．
  Expr
  get_assignments(const TpgNode* root);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
  void
  mark_tfo(const TpgNode* node);

  /// @brief 故障の影響を伝搬する値割当を求める．
  /// @param[in] node 対象のノード
  /// @return 値割り当てを表す論理式
  ///
  /// node は TFO 内のノードでかつ故障差が伝搬している．
  Expr
  record_sensitized_node(const TpgNode* node);

  /// @brief 故障の影響の伝搬を阻害する値割当を求める．
  /// @param[in] node 対象のノード
  /// @return 値割り当てを表す論理式
  ///
  /// node は TFO 内のノードかつ故障差が伝搬していない．
  Expr
  record_masking_node(const TpgNode* node);

  /// @brief side input の値を記録する．
  /// @param[in] node 対象のノード
  ///
  /// node は TFO 外のノード
  Expr
  record_side_input(const TpgNode* node);

  /// @brief 正常回路の値を返す．
  /// @param[in] node ノード
  Val3
  gval(const TpgNode* node);

  /// @brief 故障回路の値を返す．
  /// @param[in] node ノード
  Val3
  fval(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 正常値を表す変数のマップ
  const VidMap& mGvarMap;

  // 故障値を表す変数のマップ
  const VidMap& mFvarMap;

  // SAT ソルバの解
  const vector<SatBool3>& mSatModel;

  // 故障の fanout cone のマーク
  HashSet<int> mFconeMark;

  // 記録したノードの結果を格納するハッシュ表
  HashMap<int, Expr> mExprMap;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSpoList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief side input の値を記録する．
// @param[in] node 対象のノード
inline
Expr
MultiExtractor::record_side_input(const TpgNode* node)
{
  ASSERT_COND( !mFconeMark.check(node->id()) );

  VarId var(node->id());
  bool inv = (gval(node) == Val3::_0); // 0 の時に inv = true

  return Expr::literal(var, inv);
}

// @brief 正常回路の値を返す．
// @param[in] node ノード
inline
Val3
MultiExtractor::gval(const TpgNode* node)
{
  return bool3_to_val3(mSatModel[mGvarMap(node).val()]);
}

// @brief 故障回路の値を返す．
// @param[in] node ノード
inline
Val3
MultiExtractor::fval(const TpgNode* node)
{
  return bool3_to_val3(mSatModel[mFvarMap(node).val()]);
}

END_NAMESPACE_DRUID

#endif // MULTIEXTRACTOR_H
