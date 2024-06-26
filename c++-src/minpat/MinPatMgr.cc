
/// @file MinPatMgr.cc
/// @brief MinPatMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "MinPatMgr.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "MpColGraph.h"
#include "MatrixGen.h"
#include "FaultReducer.h"
#include "FaultInfo.h"
#include "TvMerger.h"
#include "ym/McMatrix.h"
#include "ym/Range.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

bool debug = false;

class MpComp :
  public McColComp
{
public:

  /// @brief コンストラクタ
  MpComp(
    const MpColGraph& graph
  );

  /// @brief 比較関数
  ///
  /// col1 の代わりに col2 を使ってもコストが上がらない時に true を返す．
  bool
  operator()(
    SizeType col1,
    SizeType col2
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  const MpColGraph& mGraph;

};

MpComp::MpComp(
  const MpColGraph& graph
) :
  mGraph{graph}
{
}

// @brief 比較関数
bool
MpComp::operator()(
  SizeType col1,
  SizeType col2
) const
{
  bool ans = mGraph.containment_check(col2, col1);
  return ans;
}

SizeType
select_Naive(
  const McMatrix& matrix,
  const MpColGraph& graph
)
{
  // matrix のアクティブな列から要素数の最も多い列を選ぶ．
  SizeType max_n = 0;
  SizeType min_c = matrix.col_size() + 1;
  SizeType max_col = 0;
  for ( auto col: matrix.col_head_list() ) {
    SizeType n = matrix.col_elem_num(col);
    if ( max_n < n ) {
      max_n = n;
      min_c = graph.conflict_num(col);
      max_col = col;
    }
    else if ( max_n == n ) {
      SizeType c = graph.conflict_num(col);
      if ( min_c > c ) {
	min_c = c;
	max_col = col;
      }
    }
  }
  return max_col;
}

SizeType
select_Simple(
  const McMatrix& matrix
)
{
  // 各行にカバーしている列数に応じた重みをつけ，
  // その重みの和が最大となる列を選ぶ．
  double max_weight = 0.0;
  SizeType max_col = 0;
  for ( auto col_pos: matrix.col_head_list() ) {
    double weight = 0.0;
    for ( auto row_pos: matrix.col_list(col_pos) ) {
      double num = matrix.row_elem_num(row_pos);
      weight += (1.0 / (num - 1.0));
    }
    weight /= matrix.col_cost(col_pos);

    if ( max_weight < weight ) {
      max_weight = weight;
      max_col = col_pos;
    }
  }
  return max_col;
}

SizeType
select_CS(
  const McMatrix& matrix
)
{
  // 各行にカバーしている列数に応じた重みをつけ，
  // その重みの和が最大となる列を選ぶ．
  SizeType nr = matrix.row_size();
  vector<double> row_weights(nr);
  for ( auto row_pos: matrix.row_head_list() ) {
    double min_cost = DBL_MAX;
    for ( auto col_pos: matrix.row_list(row_pos) ) {
      double col_cost = static_cast<double>(matrix.col_cost(col_pos)) / matrix.col_elem_num(col_pos);
      if ( min_cost > col_cost ) {
	min_cost = col_cost;
      }
    }
    row_weights[row_pos] = min_cost;
  }

  double min_delta = DBL_MAX;
  SizeType min_col = 0;

  for ( auto col_pos: matrix.col_head_list() ) {
    double col_cost = matrix.col_cost(col_pos);

    vector<int> col_delta(matrix.col_size(), 0);
    vector<SizeType> col_list;
    for ( auto row_pos: matrix.col_list(col_pos) ) {
      for ( auto col_pos1: matrix.row_list(row_pos) ) {
	if ( col_delta[col_pos1] == 0 ) {
	  col_list.push_back(col_pos1);
	}
	++ col_delta[col_pos1];
      }
    }

    vector<bool> row_mark(matrix.row_size(), false);
    vector<SizeType> row_list;
    for ( auto col_pos1: col_list ) {
      double cost1 = matrix.col_cost(col_pos1);
      SizeType num = matrix.col_elem_num(col_pos1);
      cost1 /= num;
      for ( auto row_pos: matrix.col_list(col_pos) ) {
	if ( row_weights[row_pos] < cost1 ) {
	  continue;
	}
	if ( row_mark[row_pos] ) {
	  continue;
	}
	row_mark[row_pos] = true;
	row_list.push_back(row_pos);
      }
    }

    double delta_sum = 0.0;
    for ( auto row_pos: row_list ) {
      double min_weight = DBL_MAX;
      for ( auto col_pos1: matrix.row_list(row_pos) ) {
	double n = matrix.col_elem_num(col_pos1) - col_delta[col_pos1];
	double cost1 = matrix.col_cost(col_pos1) / n;
	if ( min_weight > cost1 ) {
	  min_weight = cost1;
	}
      }
      double delta = min_weight - row_weights[row_pos];
      delta_sum += delta;
    }

    //cout << "Col#" << col->pos() << ": " << delta_sum << endl;
    if ( min_delta > delta_sum ) {
      min_delta = delta_sum;
      min_col = col_pos;
    }
  }
  return min_col;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス MinPatMgr
//////////////////////////////////////////////////////////////////////

// @brief 故障リストを縮約する．
void
MinPatMgr::fault_reduction(
  vector<const TpgFault*>& fault_list,
  const TpgNetwork& network,
  const string& algorithm,
  bool debug
)
{
  if ( algorithm != string() ) {
    FaultReducer reducer(network);
    reducer.set_debug(debug);
    reducer.fault_reduction(fault_list, algorithm);
  }
}

// テストベクタをマージして極大集合を求める．
vector<TestVector>
MinPatMgr::gen_mcsets(
  const vector<TestVector>& tv_list
)
{
  TvMerger merger(tv_list);
  return merger.gen_mcset();
}

// @brief 彩色問題でパタン圧縮を行う．
SizeType
MinPatMgr::coloring(
  const vector<const TpgFault*>& fault_list,
  const vector<TestVector>& tv_list,
  const TpgNetwork& network,
  FaultType fault_type,
  vector<TestVector>& new_tv_list
)
{
  new_tv_list.clear();
  SizeType nv = tv_list.size();
  if ( nv == 0 ) {
    return 0;
  }

  MpColGraph graph(tv_list);

  //cout << " MpColGraph generated" << endl;

  MatrixGen matgen(fault_list, tv_list, network, fault_type);
  McMatrix matrix = matgen.generate();

  //cout << " McMatrix generated" << endl;

  if ( debug ) {
    SizeType nf = matrix.active_row_num();
    cout << "# of faults: " << nf << endl;
    SizeType n_sum = 0;
    SizeType n_max = 0;
    for ( auto row: matrix.row_head_list() ) {
      SizeType n = matrix.row_elem_num(row);
      n_sum += n;
      if ( n_max < n ) {
	n_max = n;
      }
    }
    cout << "# of max detects: " << n_max << endl
	 << "# of avg. detects: " << (n_sum / static_cast<double>(nf)) << endl;
  }

  // 被覆行列の縮約を行う．
  vector<SizeType> selected_cols;
  reduce(matrix, graph, selected_cols);

  if ( debug ) {
    SizeType nf = matrix.active_row_num();
    cout << "# of reduced faults: " << nf << endl;
    SizeType n_sum = 0;
    SizeType n_max = 0;
    for ( auto row: matrix.row_head_list() ) {
      SizeType n = matrix.row_elem_num(row);
      n_sum += n;
      if ( n_max < n ) {
	n_max = n;
      }
    }
    cout << "# of max detects: " << n_max << endl
	 << "# of avg. detects: " << (n_sum / static_cast<double>(nf)) << endl;
  }

  heuristic1(matrix, graph, selected_cols);

  vector<SizeType> color_map;
  SizeType nc = graph.get_color_map(color_map);
  merge_tv_list(tv_list, nc, color_map, new_tv_list);

  // cout << "# of reduced patterns: " << nc << endl;

  return new_tv_list.size();
}

// @brief 縮約を行う．
void
MinPatMgr::reduce(
  McMatrix& matrix,
  MpColGraph& graph,
  vector<SizeType>& selected_cols
)
{
  Timer timer;

  if ( 0 ) {
    timer.reset();
    timer.start();
    cout << "reducing matrix: "
	 << matrix.active_row_num() << " x " << matrix.active_col_num()
	 << endl;
  }

  for ( ; ; ) {
    MpComp comp(graph);
    vector<SizeType> deleted_cols;
    if ( !matrix.reduce(selected_cols, deleted_cols, comp) ) {
      break;
    }
    ASSERT_COND( !selected_cols.empty() || matrix.active_row_num() > 0 );

    // 今の縮約で削除された列を衝突グラフからも削除する．
    for ( auto col: deleted_cols ) {
      graph.delete_node(col);
    }

    // 衝突グラフの変更を被覆行列に伝搬する．
    vector<SizeType> conflict_list;
    graph.get_conflict_list(deleted_cols, conflict_list);
    for ( auto col1: conflict_list ) {
      matrix.set_col_dirty(col1);
    }
  }

  if ( 0 ) {
    timer.stop();
    auto time = timer.get_time();
    cout << " ==> "
	 << matrix.active_row_num() << " x " << matrix.active_col_num()
	 << ", # of selected_cols = " << selected_cols.size()
	 << ", " << time << endl;
  }
}

// @brief ヒューリスティック1
void
MinPatMgr::heuristic1(
  McMatrix& matrix,
  MpColGraph& graph,
  vector<SizeType>& selected_cols
)
{
  while ( !selected_cols.empty() || matrix.active_row_num() > 0 ) {

    if ( 0 ) {
      cout << "matrix: " << matrix.active_row_num()
	   << " x " << matrix.active_col_num()
	   << ", selected_cols: " << selected_cols.size()
	   << endl;
    }

    // 両立集合を1つ選ぶ．
    vector<SizeType> node_list;
    get_compatible_nodes(graph, matrix, selected_cols, node_list);
    ASSERT_COND( !node_list.empty() );

    if ( 0 ) {
      cout << "chosse " << node_list.size() << " cols" << endl;
    }

    // 選ばれた両立集合に彩色を行う．
    SizeType color = graph.new_color();
    graph.set_color(node_list, color);

    // 被覆行列の更新を行う．
    for ( auto col: node_list ) {
      if ( !matrix.col_deleted(col) ) {
	matrix.select_col(col);
      }
    }

    // selected_cols の更新を行う．
    if ( !selected_cols.empty() ) {
      vector<bool> mark(matrix.col_size(), false);
      for ( auto col: node_list ) {
	mark[col] = true;
      }
      SizeType rpos = 0;
      SizeType wpos = 0;
      SizeType n = selected_cols.size();
      for ( ; rpos < n; ++ rpos ) {
	auto col = selected_cols[rpos];
	if ( !mark[col] ) {
	  selected_cols[wpos] = col;
	  ++ wpos;
	}
      }
      selected_cols.erase(selected_cols.begin() + wpos, selected_cols.end());
    }

    if ( 0 ) {
      cout << "COL#" << graph.color_num()
	   << ", # of colored columns: " << node_list.size()
	   << ": # of selected cols " << selected_cols.size() << endl
	   << " ==> " << matrix.active_row_num()
	   << " x " << matrix.active_col_num() << endl;
    }
  }
}

// @brief 両立集合を取り出す．
void
MinPatMgr::get_compatible_nodes(
  const MpColGraph& graph,
  const McMatrix& matrix,
  const vector<SizeType>& selected_nodes,
  vector<SizeType>& node_list
)
{
  vector<bool> col_mark(graph.node_num(), false);
  vector<bool> row_mark(matrix.row_size(), false);
  node_list.clear();
  if ( selected_nodes.empty() ) {
#if 0
    SizeType max_col = select_Naive(matrix, graph);
#else
    SizeType max_col = select_Simple(matrix);
#endif
    node_list.push_back(max_col);
    col_mark[max_col] = true;
    for ( auto row: matrix.col_list(max_col) ) {
      row_mark[row] = true;
    }
  }
  else {
    // 衝突数の最も少ないノードを選ぶ．
    SizeType min_c = graph.node_num() + 1;
    SizeType min_id = 0;
    for ( auto id: selected_nodes ) {
      SizeType c = graph.conflict_num(id);
      if ( min_c > c ) {
	min_c = c;
	min_id = id;
      }
    }
    node_list.push_back(min_id);
    col_mark[min_id] = true;

    // min_id に両立するノードを cand_list に入れる．
    vector<SizeType> cand_list;
    vector<SizeType> cnum_array(graph.node_num(), 0);
    cand_list.reserve(selected_nodes.size());
    for ( auto id: selected_nodes ) {
      if ( !col_mark[id] && graph.compatible_check(id, min_id) ) {
	cand_list.push_back(id);
      }
    }
    if ( !cand_list.empty() ) {
      SizeType n = cand_list.size();
      for ( auto pos1: Range(n - 1) ) {
	auto id1 = cand_list[pos1];
	for ( auto pos2: Range(pos1 + 1, n) ) {
	  auto id2 = cand_list[pos2];
	  if ( !graph.compatible_check(id1, id2) ) {
	    ++ cnum_array[id1];
	    ++ cnum_array[id2];
	  }
	}
      }
    }
    while ( !cand_list.empty() ) {
      // selected_nodes の中で node_list に両立するものを
      // 衝突数の少ない順に加える．
      SizeType min_c = graph.node_num() + 1;
      SizeType min_id = 0;
      for ( auto id: cand_list ) {
	SizeType c = cnum_array[id];
	if ( min_c > c ) {
	  min_c = c;
	  min_id = id;
	}
      }
      node_list.push_back(min_id);
      col_mark[min_id] = true;
      // min_id と衝突するノードを cand_list から除外する．
      SizeType rpos = 0;
      SizeType wpos = 0;
      SizeType n = cand_list.size();
      vector<SizeType> del_nodes;
      del_nodes.reserve(n);
      for ( ; rpos < n; ++ rpos ) {
	auto id = cand_list[rpos];
	if ( id == min_id ) {
	  continue;
	}
	if ( graph.compatible_check(id, min_id) ) {
	  cand_list[wpos] = id;
	  ++ wpos;
	}
	else {
	  del_nodes.push_back(id);
	}
      }
      // 最低でも min_id は削除される．
      cand_list.erase(cand_list.begin() + wpos, cand_list.end());
      for ( auto id1: cand_list ) {
	for ( auto id2: del_nodes ) {
	  if ( !graph.compatible_check(id1, id2) ) {
	    -- cnum_array[id1];
	  }
	}
      }
    }
  }

  // node_list に両立するノードを cand_list に入れる．
  vector<SizeType> cand_list;
  cand_list.reserve(matrix.active_col_num());
  for ( auto col: matrix.col_head_list() ) {
    if ( !col_mark[col] && graph.compatible_check(col, node_list) ) {
      cand_list.push_back(col);
    }
  }
  for ( ; ; ) {
    // cand_list の中で新たに被覆する行の最も多いものを選ぶ．
    SizeType max_num = 0;
    SizeType max_col = 0;
    for ( auto col: cand_list ) {
      SizeType num = 0;
      for ( auto row: matrix.col_list(col) ) {
	if ( !row_mark[row] ) {
	  ++ num;
	}
      }
      if ( max_num < num ) {
	max_num = num;
	max_col = col;
      }
    }
    if ( max_col == -1 ) {
      // なければ終わる．
      break;
    }

    // max_col を選ぶ．
    node_list.push_back(max_col);
    for ( auto row: matrix.col_list(max_col) ) {
      row_mark[row] = true;
    }

    // max_col と衝突するノードを cand_list から除外する．
    SizeType rpos = 0;
    SizeType wpos = 0;
    SizeType n = cand_list.size();
    for ( ; rpos < n; ++ rpos ) {
      auto col = cand_list[rpos];
      if ( col != max_col && graph.compatible_check(col, max_col) ) {
	cand_list[wpos] = col;
	++ wpos;
      }
    }
    // 必ず wpos < n
    cand_list.erase(cand_list.begin() + wpos, cand_list.end());
  }
}

// @brief 彩色結果から新しいテストパタンのリストを生成する．
void
MinPatMgr::merge_tv_list(
  const vector<TestVector>& tv_list,
  SizeType nc,
  const vector<SizeType>& color_map,
  vector<TestVector>& new_tv_list
)
{
  SizeType nv = tv_list.size();
  vector<vector<SizeType>> tvgroup_list(nc);
  for ( auto tvid: Range(nv) ) {
    SizeType c = color_map[tvid];
    if ( c > 0 ) {
      tvgroup_list[c - 1].push_back(tvid);
    }
  }

  new_tv_list.clear();
  for ( auto new_id: Range(nc) ) {
    const vector<SizeType>& id_list = tvgroup_list[new_id];
    ASSERT_COND( id_list.size() > 0 );
    TestVector tv = tv_list[id_list[0]];
    for ( auto i: Range(1, id_list.size()) ) {
      tv &= tv_list[id_list[i]];
    }
    new_tv_list.push_back(tv);
  }
}

END_NAMESPACE_DRUID
