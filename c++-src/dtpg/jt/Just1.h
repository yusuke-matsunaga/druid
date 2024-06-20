#ifndef JUST1_H
#define JUST1_H

/// @file Just1.h
/// @brief Just1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "JustImpl.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class Just1 Just1.h "td/Just1.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just1 :
  public JustImpl
{
public:

  /// @brief コンストラクタ
  Just1(
    SizeType max_id ///< [in] ノード番号の最大値
  );

  /// @brief デストラクタ
  ~Just1();


private:
  //////////////////////////////////////////////////////////////////////
  // JustImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化処理
  void
  just_init(
    const NodeTimeValList& assign_list ///< [in] 割当リスト
  ) override;

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @return 選んだファンインのノードを返す．
  const TpgNode*
  select_cval_node(
    const TpgNode* node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) override;

  /// @brief 終了処理
  void
  just_end() override;

};

END_NAMESPACE_DRUID

#endif // JUST1_H
