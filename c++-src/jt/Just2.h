#ifndef JUST2_H
#define JUST2_H

/// @file Just2.h
/// @brief Just2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "JustImpl.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class Just2 Just2.h "td/Just2.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just2 :
  public JustImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  Just2(int max_id);

  /// @brief デストラクタ
  virtual
  ~Just2();


private:
  //////////////////////////////////////////////////////////////////////
  // JustImpl の仮想関数
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
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 重みの計算を行う．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  add_weight(const JustData& jd,
	     const TpgNode* node,
	     int time);

  /// @brief 見積もり値の計算を行う．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  calc_value(const JustData& jd,
	     const TpgNode* node,
	     int time);

  /// @brief 重みを考えた価値を返す．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  double
  node_value(const TpgNode* node,
	     int time) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノードのリスト
  // 作業領域のクリアで用いる．
  vector<const TpgNode*> mNodeList[2];

  // 重み配列
  vector<int> mWeightArray;

  // 作業用の配列
  vector<double> mTmpArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 重みを考えた価値を返す．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
double
Just2::node_value(const TpgNode* node,
		  int time) const
{
  int index = node->id() * 2 + time;
  ASSERT_COND ( mWeightArray[index] > 0 );

  return mTmpArray[index] / mWeightArray[index];
}

END_NAMESPACE_DRUID

#endif // JUST2_H
