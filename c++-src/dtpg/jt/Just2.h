#ifndef JUST2_H
#define JUST2_H

/// @file Just2.h
/// @brief Just2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "JustBase.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Just2 Just2.h "td/Just2.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just2 :
  public JustBase
{
public:

  /// @brief コンストラクタ
  Just2(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~Just2();


private:
  //////////////////////////////////////////////////////////////////////
  // JustImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化処理
  void
  just_init(
    const AssignList& assign_list ///< [in] 割当リスト
  ) override;

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @return 選んだファンインのノードを返す．
  TpgNode
  select_cval_node(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) override;

  /// @brief 終了処理
  void
  just_end() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 重みの計算を行う．
  void
  add_weight(
    const TpgNode& node, ///< [in] タイムフレーム ( 0 or 1 )
    int time
  );

  /// @brief 見積もり値の計算を行う．
  void
  calc_value(
    const TpgNode& node, ///< [in] タイムフレーム ( 0 or 1 )
    int time
  );

  /// @brief 重みを考えた価値を返す．
  double
  node_value(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  ) const
  {
    SizeType index = node.id() * 2 + time;
    ASSERT_COND ( mWeightArray[index] > 0 );

    return mTmpArray[index] / mWeightArray[index];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードのリスト
  // 作業領域のクリアで用いる．
  std::vector<TpgNode> mNodeList[2];

  // 重み配列
  std::vector<int> mWeightArray;

  // 作業用の配列
  std::vector<double> mTmpArray;

};

END_NAMESPACE_DRUID

#endif // JUST2_H
