#ifndef DSATUR_H
#define DSATUR_H

/// @file Dsatur.h
/// @brief Dsatur のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ColGraph.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dsatur Dsatur.h "Dsatur.h"
/// @brief 彩色問題を dsatur アルゴリズムで解くためのクラス
//////////////////////////////////////////////////////////////////////
class Dsatur
{
public:

  /// @brief コンストラクタ
  Dsatur(
    ColGraph& graph ///< [in] 対象のグラフ
  );

  /// @brief デストラクタ
  ~Dsatur();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 彩色する．
  void
  coloring();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  void
  init();

  /// @brief (sat_degree, adj_degree) の辞書順で最大のノードを取ってくる．
  SizeType
  get_max_node();

  /// @brief node_id に color の色を割り当て情報を更新する．
  void
  update(
    SizeType node_id, ///< [in] ノード番号
    SizeType color    ///< [in] 色
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  ColGraph& mGraph;

  // 彩色の候補のノードリスト
  vector<SizeType> mCandList;

};

END_NAMESPACE_DRUID

#endif // DSATUR_H
