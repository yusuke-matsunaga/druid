#ifndef DPATGRAPH_H
#define DPATGRAPH_H

/// @file DPatGraph.h
/// @brief DPatGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/PackedVal.h"
#include "POSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DPatGraph DPatGraph.h "DPatGraph.h"
/// @brief PackedVal の支配関係を表すグラフ
///
/// pat1 が pat2 に含まれるとき pat1 が pat2 を支配すると定義する．
/// これだけならただのビットワイズ演算でチェック可能だが，
/// ここでは与えられたパタンの集合の支配関係をグラフにして直接の支配関係
/// のみを取り出す．
/// つまり，推移律を用いて導かれる支配関係は記録しない．
//////////////////////////////////////////////////////////////////////
class DPatGraph
{
public:

  /// @brief コンストラクタ
  DPatGraph(
    const std::vector<PackedVal>& pat_list ///< [in] パタンのリスト
  );

  /// @brief デストラクタ
  ~DPatGraph() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタンのリストを返す．
  const std::vector<PackedVal>&
  pat_list() const
  {
    return mPatList;
  }

  /// @brief 直接の支配関係にあるパタンのリストを返す．
  ///
  /// - ただしブロックされたパタンに到達した場合はそこで止まる．
  std::vector<PackedVal>
  dom_list(
    PackedVal pat,                             ///< [in] 始点となるパタン
    const std::vector<PackedVal>& target_pats, ///< [in] ターゲットのパタンのリスト
    const std::vector<PackedVal>& block_pats   ///< [in] ブロックするパタンのリスト
  ) const;

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief dom_list() 用の下請け関数
  void
  dfs(
    SizeType id,
    std::vector<int>& mark,
    std::vector<PackedVal>& ans_list
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パタンのリスト
  std::vector<PackedVal> mPatList;

  // パタンをキーにしてノード番号を持つ辞書
  // mPatList の逆関数
  std::unordered_map<PackedVal, SizeType> mIdMap;

  // 半順序構造
  POSet mPOSet;

};

END_NAMESPACE_DRUID

#endif // DPATGRAPH_H
