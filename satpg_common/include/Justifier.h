﻿#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "TpgNode.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "td/Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ID番号の最大値
  Justifier(int max_id);

  /// @brief デストラクタ
  virtual
  ~Justifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var_map 変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @return 外部入力上の値の割当リスト
  NodeValList
  operator()(const NodeValList& assign_list,
	     const VidMap& var_map,
	     const vector<SatBool3>& model);

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var1_map 1時刻目の変数番号のマップ
  /// @param[in] var2_map 2時刻目の変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @return 外部入力上の値の割当リスト
  NodeValList
  operator()(const NodeValList& assign_list,
	     const VidMap& var1_map,
	     const VidMap& var2_map,
	     const vector<SatBool3>& model);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化処理
  /// @param[in] assign_list 割当リスト
  /// @param[in] jd justify 用のデータ
  virtual
  void
  just_init(const NodeValList& assign_list,
	    const JustData& jd) = 0;

  /// @brief 正当化処理
  /// @param[in] jd justiry用のデータ
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[in] pi_assign_list 結果の割当を保持するリスト
  void
  justify(const JustData& jd,
	  const TpgNode* node,
	  int time,
	  NodeValList& pi_assign_list);

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @param[in] jd justiry用のデータ
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @return 選んだファンインのノードを返す．
  virtual
  const TpgNode*
  select_cval_node(const JustData& jd,
		   const TpgNode* node,
		   int time) = 0;

  /// @brief 終了処理
  virtual
  void
  just_end() = 0;

   /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  set_mark(const TpgNode* node,
	   int time);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  bool
  mark(const TpgNode* node,
       int time) const;

  /// @brief 全てのマークを消す．
  void
  clear_mark();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

};

/// @brief Just1 を生成する．
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just1(int max_id);

/// @brief Just2 を生成する．
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just2(int max_id);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief justified マークをつける．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
void
Justifier::set_mark(const TpgNode* node,
		    int time)
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  mMarkArray[node->id()] |= (1U << time);
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
bool
Justifier::mark(const TpgNode* node,
		int time) const
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  return static_cast<bool>((mMarkArray[node->id()] >> time) & 1U);
}

END_NAMESPACE_YM_SATPG

#endif // JUSTIFIER_H
