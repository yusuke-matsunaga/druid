#ifndef MPCOLGRAPH_H
#define MPCOLGRAPH_H

/// @file MpColGraph.h
/// @brief MpColGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpColGraph MpColGraph.h "MpColGraph.h"
/// @brief MinPatMgr の coloring 用のグラフを表すクラス
///
/// * 隣接ペア(n1, n2)の枝を明示的には持たない．
/// * 代わりにテストベクタの各ビットごとに相反するグループのリストを持つ．
/// * テストベクタ(グラフのノード)は一時的に削除される．
//////////////////////////////////////////////////////////////////////
class MpColGraph
{
public:

  /// @brief コンストラクタ
  MpColGraph(
    const vector<TestVector>& tv_list ///< [in] テストパタンのリスト
  );

  /// @brief デストラクタ
  ~MpColGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を返す．
  SizeType
  node_num() const
  {
    return mNodeNum;
  }

  /// @brief node1 と node2 が両立する時 true を返す．
  bool
  compatible_check(
    SizeType node1, ///< [in] ノード番号1
    SizeType node2  ///< [in] ノード番号2
  ) const
  {
    return compatible_check(node1, vector<SizeType>{node2});
  }

  /// @brief node が node_list のノード集合と両立する時 true を返す．
  bool
  compatible_check(
    SizeType node,               ///< [in] ノード番号
    const vector<SizeType>& node_list ///< [in] ノード番号のリスト
  ) const;

  /// @brief node1 の衝突集合が node2 の衝突集合に含まれていたら true を返す．
  bool
  containment_check(
    SizeType node1, ///< [in] ノード番号1
    SizeType node2  ///< [in] ノード番号2
  ) const;

  /// @brief ノードの衝突数を返す．
  ///
  /// 削除されたノードはカウントしない．
  SizeType
  conflict_num(
    SizeType node ///< [in] ノード番号
  ) const;

  /// @brief ノードの衝突リストを返す．
  void
  get_conflict_list(
    SizeType node,                   ///< [in] ノード番号
    vector<SizeType>& conflict_list  ///< [out] 衝突リスト
  ) const
  {
    get_conflict_list(vector<SizeType>{node}, conflict_list);
  }

  /// @brief ノードの衝突リストを返す．
  void
  get_conflict_list(
    const vector<SizeType>& node_list, ///< [in] ノード番号のリスト
    vector<SizeType>& conflict_list    ///< [out] 衝突リスト
  ) const;

  /// @brief ノードを削除する．
  void
  delete_node(
    SizeType node ///< [in] 削除するノード番号
  );

  /// @brief 色数を返す．
  SizeType
  color_num() const
  {
    return mColNum;
  }

  /// @brief ノードの色を得る．
  SizeType
  color(
    SizeType node ///< [in] ノード番号 ( 0 <= node < node_num() )
  ) const
  {
    ASSERT_COND( node >= 0 && node < node_num() );

    return mColorMap[node];
  }

  /// @brief 新しい色を割り当てる．
  /// @return 新しい色番号を返す．
  SizeType
  new_color()
  {
    return ++ mColNum;
  }

  /// @brief ノードを色をつける．
  void
  set_color(
    SizeType node,  ///< [in] ノード番号 ( 0 <= node < node_num() )
    SizeType color  ///< [in] 色番号 ( 1 <= color <= color_num() )
  )
  {
    ASSERT_COND( 0 <= node && node < node_num() );
    ASSERT_COND( 1 <= color && color <= color_num() );

    mColorMap[node] = color;

    delete_node(node);
  }

  /// @brief ノードの集合を色をつける．
  void
  set_color(
    const vector<SizeType>& node_list, ///< [in] ノード番号のリスト
    SizeType color ///< [in] 色番号 ( 1 <= color <= color_num() )
  )
  {
    for ( auto node: node_list ) {
      set_color(node, color);
    }
  }

  /// @brief color_map を作る．
  SizeType
  get_color_map(
    vector<SizeType>& color_map
  ) const;

  /// @brief 隣接しているノード対に同じ色が割り当てられていないか確認する．
  bool
  verify() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 衝突リストを作る．
  ///< [in] tv_list テストパタンのリスト
  void
  gen_conflict_list();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタのリスト
  const vector<TestVector>& mTvList;

  // ノード(テストベクタ)数
  SizeType mNodeNum;

  // テストベクタのビット長
  SizeType mVectorSize;

  // 衝突関係にあるノード番号のリストの配列
  // サイズはテストベクタのベクタ長 x 2
  vector<vector<SizeType>> mNodeListArray;

  // 衝突関係にあるノード集合を表す OID のリスト
  // サイズは mNodeNum
  vector<vector<SizeType>> mOidListArray;

  // 現在使用中の色数
  SizeType mColNum;

  // 彩色結果の配列
  // サイズは mNodeNum
  vector<SizeType> mColorMap;

  // 作業用に用いる配列
  // サイズは mNodeNum;
  mutable
  vector<SizeType> mTmpMark;

  // 作業用に用いるリスト
  mutable
  vector<SizeType> mTmpList;

};

#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード数を返す．
inline
int
MpColGraph::node_num() const
{
  return mNodeNum;
}

// @brief node1 と node2 が両立する時 true を返す．
// @param[in] node1, nod2 ノード番号
inline
bool
MpColGraph::compatible_check(int node1,
			     int node2) const
{
  return compatible_check(node1, vector<int>(1, node2));
}

// @brief 色数を返す．
inline
int
MpColGraph::color_num() const
{
  return mColNum;
}

// @brief ノードの色を得る．

inline
int
MpColGraph::color(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  return mColorMap[node_id];
}

// @brief 新しい色を割り当てる．
// @return 新しい色番号を返す．
inline
int
MpColGraph::new_color()
{
  return ++ mColNum;
}

// @brief ノードを色をつける．
// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
// @param[in] color 色番号 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(int node_id,
		      int color)
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );
  ASSERT_COND( color >= 1 && color <= color_num() );

  mColorMap[node_id] = color;

  delete_node(node_id);
}

// @brief ノードの集合を色をつける．
// @param[in] node_id_list ノード番号のリスト
// @param[in] color 色 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(const vector<int>& node_id_list,
		      int color)
{
  ASSERT_COND( color >= 1 && color <= color_num() );

  for ( auto node_id: node_id_list ) {
    set_color(node_id, color);
  }
}
#endif

END_NAMESPACE_DRUID

#endif // MPCOLGRAPH_H
