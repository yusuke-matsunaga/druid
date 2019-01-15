#ifndef ISX2_H
#define ISX2_H

/// @file Isx2.h
/// @brief Isx2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "MpColGraph.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Isx2 Isx2.h "Isx2.h"
/// @brief independent set extraction を行うクラス
//////////////////////////////////////////////////////////////////////
class Isx2
{
public:

  /// @brief コンストラクタ
  /// @param[in] graph 対象のグラフ
  Isx2(MpColGraph& graph);

  /// @brief デストラクタ
  ~Isx2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief independent set extraction を用いた coloring を行う．
  /// @param[in] limit 残りのノード数がこの値を下回ったら処理をやめる．
  /// @return 彩色数を返す．
  ///
  /// ここでは部分的な彩色を行う．
  int
  coloring(int limit);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief maximal independent set を選ぶ．
  ///
  /// - 結果は indep_set に格納される．
  /// - mRandGen を用いてランダムに選ぶ．
  void
  get_indep_set(vector<int>& indep_set);

  /// @brief mCandList, mCandMark を初期化する．
  void
  init_cand_list();

  /// @brief 集合に加えるノ選ぶ．
  ///
  /// - 独立集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
  /// - 追加できるノードがない場合は -1 を返す．
  int
  select_node();

  /// @brief 集合に加えるノ選ぶ．
  ///
  /// - 独立集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
  /// - 追加できるノードがない場合は -1 を返す．
  int
  select_node0();

  /// @brief 候補リストを更新する．
  /// @param[in] node 新たに加わったノード
  void
  update_cand_list(int node_id);

  /// @brief ランダムに選択する．
  int
  random_select(const vector<int>& cand_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  MpColGraph& mGraph;

  // 候補ノードのリスト
  vector<int> mCandList;

  // 候補ノードの印
  // サイズは node_num()
  vector<bool> mCandMark;

  // 候補ノードの隣接数
  // サイズは node_num()
  vector<int> mAdjCount;

  // get_indep_set() で用いる一時的な行の被覆フラグ
  // サイズは mGraph.fault_num()
  vector<bool> mCoverFlag;

  // 候補ノードの価値
  // サイズは node_num()
  vector<int> mValue;

  // 乱数生成器
  RandGen mRandGen;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ランダムに選択する．
inline
int
Isx2::random_select(const vector<int>& cand_list)
{
  int n = cand_list.size();
  if ( n == 0 ) {
    return -1;
  }
  int r = mRandGen.int32() % n;
  return cand_list[r];
}

END_NAMESPACE_DRUID

#endif // ISX2_H
