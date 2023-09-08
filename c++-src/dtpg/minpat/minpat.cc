
/// @file minpat.cc
/// @brief minpat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "TestVector.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "MpColGraph.h"
#include "MatrixGen.h"
#include "ym/McMatrix.h"
#include "ym/Range.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// @brief 彩色結果から新しいテストパタンのリストを生成する．
void
merge_tv_list(const vector<TestVector>& tv_list,
	      int nc,
	      const vector<int>& color_map,
	      vector<TestVector>& new_tv_list)
{
  int nv = tv_list.size();
  vector<vector<int>> tvgroup_list(nc);
  for ( auto tvid: Range(nv) ) {
    int c = color_map[tvid];
    if ( c > 0 ) {
      tvgroup_list[c - 1].push_back(tvid);
    }
  }

  new_tv_list.clear();
  for ( auto new_id: Range(nc) ) {
    const vector<int>& id_list = tvgroup_list[new_id];
    ASSERT_COND( id_list.size() > 0 );
    TestVector tv = tv_list[id_list[0]];
    for ( auto i: Range(1, id_list.size()) ) {
      tv &= tv_list[id_list[i]];
    }
    new_tv_list.push_back(tv);
  }
}

class MpComp :
  public McColComp
{
public:

  /// @brief コンストラクタ
  MpComp(const MpColGraph& graph);

  /// @brief 比較関数
  ///
  /// col1 の代わりに col2 を使ってもコストが上がらない時に true を返す．
  bool
  operator()(int col1,
	     int col2) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  const MpColGraph& mGraph;

};

MpComp::MpComp(const MpColGraph& graph) :
  mGraph(graph)
{
}

// @brief 比較関数
bool
MpComp::operator()(int col1,
		   int col2) const
{
  return mGraph.containment_check(col2, col1);
}

void
get_compatible_nodes(const MpColGraph& graph,
		     const McMatrix& matrix,
		     vector<int>& node_list)
{

}

END_NONAMESPACE


// @brief 彩色問題でパタン圧縮を行う．
// @param[in] tv_list 初期テストパタンのリスト
// @param[out] new_tv_list 圧縮結果のテストパタンのリスト
// @return 結果のパタン数を返す．
int
minpat(const vector<const TpgFault*>& fault_list,
       const vector<TestVector>& tv_list,
       const TpgNetwork& network,
       FaultType fault_type,
       vector<TestVector>& new_tv_list)
{
  new_tv_list.clear();
  int nv = tv_list.size();
  if ( nv == 0 ) {
    return 0;
  }

  cout << "*** coloring ***" << endl;
  cout << "# of initial patterns: " << nv << endl;

  MpColGraph graph(tv_list);

  cout << " MpColGraph generated" << endl;

  MatrixGen matgen(fault_list, tv_list, network, fault_type);
  std::unique_ptr<McMatrix> matrix = matgen.generate();

  cout << " McMatrix generated" << endl;

  MpComp comp(graph);
  for ( ; ; ) {
#if 0
    vector<int> selected_cols;
    graph.reduce(selected_cols, comp);

    if ( matrix->active_row_num() == 0 ) {
      break;
    }
#endif

    vector<int> node_list;
    get_compatible_nodes(graph, *matrix, node_list);

    int color = graph.new_color();
    graph.set_color(node_list, color);

    for ( auto col: node_list ) {
      matrix->select_col(col);
    }

    if ( matrix->active_row_num() == 0 ) {
      break;
    }

    for ( auto col: Range(nv) ) {
      if ( matrix->col_deleted() ) {
	graph.delete_node(col);
      }
    }
  }

  vector<int> color_map;
  int nc = graph.get_color_map(color_map);
  merge_tv_list(tv_list, nc, color_map, new_tv_list);

  cout << "# of reduced patterns: " << nc << endl;

  return nc;
}

END_NAMESPACE_DRUID
