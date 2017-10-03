﻿#ifndef SA_EXTRACTOR_H
#define SA_EXTRACTOR_H

/// @file sa/Extractor.h
/// @brief Extractor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014 Yusuke Matsunaga
/// All rights reserved.


#include "sa/sa_nsdef.h"
#include "../../dtpg/ValMap.h"
#include "ym/HashSet.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
/// @class Extractor Extractor.h "sa/Extractor.h"
/// @brief 十分割当を求めるクラス
//////////////////////////////////////////////////////////////////////
class Extractor
{
public:

  /// @brief コンストラクタ
  /// @param[in] val_map ノードの値割当を保持するクラス
  Extractor(const ValMap& val_map);

  /// @brief デストラクタ
  ~Extractor();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割当を求める．
  /// @param[in] fault 故障
  /// @param[out] assign_list 値の割当リスト
  void
  operator()(const TpgFault* fault,
	     NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の影響を伝搬するノードを求める．
  /// @param[in] node 対象のノード
  /// @return node から外部出力まで故障の影響が伝搬していたら true を返す．
  const TpgNode*
  find_sensitized_output(const TpgNode* node);

  /// @brief 故障の影響の伝搬を阻害する値割当を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  void
  record_sensitized_node(const TpgNode* node,
			 NodeValList& assign_list);

  /// @brief side inputs の値を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  void
  record_side_inputs(const TpgNode* node,
		     NodeValList& assign_list);

  /// @brief 故障の影響の伝搬を阻害する値割当を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  void
  record_masking_node(const TpgNode* node,
		      NodeValList& assign_list);

  /// @brief ノードの割当を記録する．
  /// @param[in] node 対象のノード
  /// @param[out] assign_list 値割当を記録するリスト
  void
  record_node(const TpgNode* node,
	      NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値割当を保持するクラス
  const ValMap& mValMap;

  // 故障の fanout cone のマーク
  HashSet<ymuint> mFconeMark;

  // 記録済みノードを保持するハッシュ表
  HashSet<ymuint> mRecorded;

};

END_NAMESPACE_YM_SATPG_SA

#endif // SA_EXTRACTOR_H
