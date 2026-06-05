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


BEGIN_NAMESPACE_DRUID

class DPatNode;

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

  /// @brief 空のコンストラクタ
  DPatGraph() = default;

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
  /// pat はこのグラフに含まれていない場合がある．
  std::vector<PackedVal>
  dom_list(
    PackedVal pat ///< [in] 対象のパタン
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
    const DPatNode* node,
    PackedVal pat,
    std::unordered_set<PackedVal>& mark,
    std::vector<PackedVal>& ans_list
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パタンのリスト
  std::vector<PackedVal> mPatList;

  // ノードのリスト
  std::vector<std::unique_ptr<DPatNode>> mNodeList;

  // パタンをキーにして DPatNode を持つ辞書
  std::unordered_map<PackedVal, DPatNode*> mNodeMap;

  // ランク0のノードのリスト
  std::vector<DPatNode*> mRank0List;

};


//////////////////////////////////////////////////////////////////////
/// @class DPatNode DPatGraph.h "DPatGraph.h"
/// @brief DPatGraph のノードを表すクラス
//////////////////////////////////////////////////////////////////////
class DPatNode
{
  friend class DPatGraph;

public:

  /// @brief コンストラクタ
  DPatNode(
    SizeType id,  ///< [in] ID番号
    PackedVal pat ///< [in] パタン
  ) : mId{id},
      mPat{pat}
  {
  }

  /// @brief デストラクタ
  ~DPatNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す.
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief パタンを返す．
  PackedVal
  pat() const
  {
    return mPat;
  }

  /// @brief 直接支配するパタンのリストを返す．
  std::vector<PackedVal>
  dom_list() const;

  /// @brief ランクを返す．
  SizeType
  rank() const
  {
    return mRank;
  }

  /// @grief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号
  SizeType mId;

  // パタン
  PackedVal mPat;

  // ランク
  SizeType mRank{0};

  bool mHasRank{false};

  SizeType mCount{0};

  // 支配する故障のリスト
  std::vector<DPatNode*> mDownLink;

  // 支配している故障のリスト
  std::vector<DPatNode*> mUpLink;

};

END_NAMESPACE_DRUID

#endif // DPATGRAPH_H
