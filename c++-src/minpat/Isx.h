#ifndef ISX_H
#define ISX_H

/// @file Isx.h
/// @brief Isx のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ColGraph.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Isx Isx.h "Isx.h"
/// @brief Independent Set eXtraction を行うクラス
//////////////////////////////////////////////////////////////////////
class Isx
{
public:

  /// @brief コンストラクタ
  Isx(
    ColGraph& graph,        ///< [in] 対象のグラフ
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~Isx() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief independent set extraction を用いた coloring を行う．
  ///
  /// 残りのノード数が limit を下回ったら処理をやめる．
  void
  coloring(
    SizeType limit ///< [in] しきい値
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief maximal independent set を選ぶ．
  /// @return 求めた independent set を返す．
  ///
  /// 選択時に複数の候補があるときはランダムに選ぶ．
  vector<SizeType>
  get_indep_set();

  /// @brief 独立集合に加えるノードを選ぶ．
  /// @return ノード番号を返す．
  ///
  /// - 独立集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
  SizeType
  select_node(
    const vector<SizeType>& cand_list, ///< [in] 候補ノードのリスト
    const vector<SizeType>& indep_set  ///< [in] 現在の独立集合
  );

  /// @brief ランダムに選択する．
  SizeType
  random_select(
    const vector<SizeType>& cand_list ///< [in] 候補のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  ColGraph& mGraph;

  // 候補ノードのリスト
  vector<SizeType> mCandList;

  // 候補ノードの印
  // サイズは node_num()
  vector<bool> mCandMark;

  // 候補ノードの隣接数
  // サイズは node_num()
  vector<SizeType> mAdjCount;

  // 候補ノードの価値
  // サイズは node_num()
  vector<SizeType> mValue;

  // 乱数生成器
  std::mt19937 mRandGen;

  // スキップフラグ
  bool mSkip{false};

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // ISX_H
