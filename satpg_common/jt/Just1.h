﻿#ifndef JUST1_H
#define JUST1_H

/// @file Just1.h
/// @brief Just1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Justifier.h"


BEGIN_NAMESPACE_YM_SATPG

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class Just1 Just1.h "td/Just1.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just1 :
  public Justifier
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  Just1(int max_id);

  /// @brief デストラクタ
  virtual
  ~Just1();


private:
  //////////////////////////////////////////////////////////////////////
  // Justifier の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化処理
  /// @param[in] assign_list 割当リスト
  /// @param[in] jd justify 用のデータ
  virtual
  void
  just_init(const NodeValList& assign_list,
	    const JustData& jd) override;

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @param[in] jd justiry用のデータ
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @return 選んだファンインのノードを返す．
  virtual
  const TpgNode*
  select_cval_node(const JustData& jd,
		   const TpgNode* node,
		   int time) override;

  /// @brief 終了処理
  virtual
  void
  just_end() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // JUST1_H
