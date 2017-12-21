﻿#ifndef EXTRACTOR_H
#define EXTRACTOR_H

/// @file Extractor.h
/// @brief Extractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ValMap.h"
#include "ym/HashSet.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Extractor Extractor.h "Extractor.h"
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
/// という選択で答が変わりうる．
//////////////////////////////////////////////////////////////////////
class Extractor
{
public:

  /// @brief コンストラクタ
  Extractor();

  /// @brief デストラクタ
  ~Extractor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割当を求める．
  /// @param[in] root 起点となるノード
  /// @param[in] val_map 値割り当ての結果を保持するオブジェクト
  /// @param[out] assign_list 値の割当リスト
  void
  operator()(const TpgNode* root,
	     const ValMap& val_map,
	     NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node の TFO に印をつけ，故障差の伝搬している外部出力を求める．
  void
  mark_tfo(const TpgNode* node);

  /// @brief 故障の影響の伝搬する値割当を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  ///
  /// node は TFO 内のノードでかつ故障差が伝搬している．
  void
  record_sensitized_node(const TpgNode* node,
			 NodeValList& assign_list);

  /// @brief 故障の影響の伝搬を阻害する値割当を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  ///
  /// node は TFO 内のノードかつ故障差が伝搬していない．
  void
  record_masking_node(const TpgNode* node,
		      NodeValList& assign_list);

  /// @brief side input の値を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  ///
  /// node は TFO 外のノード
  void
  record_side_input(const TpgNode* node,
		    NodeValList& assign_list);

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

  // 値割当を保持するクラス
  const ValMap* mValMapPtr;

  // 故障の fanout cone のマーク
  HashSet<ymuint> mFconeMark;

  // 記録済みノードを保持するハッシュ表
  HashSet<ymuint> mRecorded;

  // 故障差の伝搬している外部出力のリスト
  vector<const TpgNode*> mSpoList;

};

// @brief 正常回路の値を返す．
// @param[in] node ノード
inline
Val3
Extractor::gval(const TpgNode* node)
{
  ASSERT_COND( mValMapPtr != nullptr );
  return mValMapPtr->gval(node);
}

// @brief 故障回路の値を返す．
// @param[in] node ノード
inline
Val3
Extractor::fval(const TpgNode* node)
{
  ASSERT_COND( mValMapPtr != nullptr );
  return mValMapPtr->fval(node);
}

END_NAMESPACE_YM_SATPG

#endif // EXTRACTOR_H