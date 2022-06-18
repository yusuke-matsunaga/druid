
/// @file ColCov.cc
/// @brief ColCov の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ColCov.h"
#include "ym/MinCov.h"
#include "ym/UdGraph.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス ColCov
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ColCov::ColCov() :
  mRowSize{0},
  mColSize{0}
{
}

// @brief コンストラクタ
ColCov::ColCov(
  SizeType row_size,
  SizeType col_size
) : mRowSize{row_size},
    mColSize{col_size}
{
}

// @brief デストラクタ
ColCov::~ColCov()
{
}

// @brief サイズを設定する．
void
ColCov::resize(
  SizeType row_size,
  SizeType col_size
)
{
  mRowSize = row_size;
  mColSize = col_size;

  mElemList.clear();
  mConflictList.clear();
}

// @brief 行列の要素を追加する．
void
ColCov::insert_elem(
  SizeType row_pos,
  SizeType col_pos
)
{
  ASSERT_COND( row_pos >= 0 && row_pos < row_size() );
  ASSERT_COND( col_pos >= 0 && col_pos < col_size() );

  mElemList.push_back(make_pair(row_pos, col_pos));
}

// @brief 列の衝突関係を追加する．
void
ColCov::insert_conflict(
  SizeType col_pos1,
  SizeType col_pos2)
{
  ASSERT_COND( col_pos1 >= 0 && col_pos1 < col_size() );
  ASSERT_COND( col_pos2 >= 0 && col_pos2 < col_size() );

  if ( col_pos1 > col_pos2 ) {
    // col_pos1 < col_pos2 になるように正規化する．
    SizeType tmp = col_pos1;
    col_pos1 = col_pos2;
    col_pos2 = tmp;
  }
  mConflictList.push_back(make_pair(col_pos1, col_pos2));
}

// @brief ヒューリスティックで解を求める．
int
ColCov::heuristic(
  const string& algorithm,
  const string& option,
  vector<int>& color_map
)
{
  cout << "# of rows      = " << row_size() << endl;
  cout << "# of columns   = " << col_size() << endl;
  cout << "# of conflicts = " << mConflictList.size() << endl;

  int col_num = 0;

  if ( algorithm == "mincov+color" ) {
    col_num = mincov_color(option, color_map);
  }
  else {
    col_num = mincov_color(option, color_map);
  }

  vector<int> uncov_list;
  bool stat1 = check_cover(color_map, uncov_list);
  if ( !stat1 ) {
    cout << "check_cover() error" << endl;
  }
  bool stat2 = check_conflict(color_map);
  if ( !stat2 ) {
    cout << "check_conflict() error" << endl;
  }

  return col_num;
}

// @brief 最初に MinCov を行いその後で coloring を行う．
int
ColCov::mincov_color(
  const string& option,
  vector<int>& color_map
)
{
  MinCov mincov(row_size(), col_size());
  for ( auto p: mElemList ) {
    mincov.insert_elem(p.first, p.second);
  }

  // 列を選ぶ．
  vector<SizeType> sel_cols;
  mincov.heuristic(sel_cols);

  // 選ばれた列を前に詰める．
  SizeType new_col_size = sel_cols.size();
  vector<SizeType> col_map(col_size(), -1);
  vector<SizeType> rcol_map(new_col_size);
  for ( auto new_col: Range(new_col_size) ) {
    auto col = sel_cols[new_col];
    col_map[col] = new_col;
    rcol_map[new_col] = col;
  }


  SizeType ne = 0;
  UdGraph graph(new_col_size);
  for ( auto p: mConflictList ) {
    auto col1 = p.first;
    auto col2 = p.second;
    auto new_col1 = col_map[col1];
    auto new_col2 = col_map[col2];
    if ( new_col1 != -1 && new_col2 != -1 ) {
      graph.connect(new_col1, new_col2);
      ++ ne;
    }
  }

  cout << "After mincov" << endl
       << "# of columns   = " << new_col_size << endl
       << "# of conflicts = " << ne << endl;

  vector<SizeType> _color_map;
  SizeType col_num = graph.coloring(_color_map, "isx");
  color_map.clear();
  color_map.resize(col_size(), -1);
  for ( auto new_col: Range(new_col_size) ) {
    auto col = rcol_map[new_col];
    color_map[col] = _color_map[new_col];
  }

  return col_num;
}

// @brief 被覆解になっているか調べる．
bool
ColCov::check_cover(
  const vector<int>& color_map,
  vector<int>& uncov_list
)
{
  vector<bool> col_mark(col_size(), false);
  for ( auto col: Range(col_size()) ) {
    if ( color_map[col] != -1 ) {
      col_mark[col] = true;
    }
  }

  vector<bool> row_mark(row_size(), false);
  for ( auto p: mElemList ) {
    auto row = p.first;
    auto col = p.second;
    if ( col_mark[col] ) {
      row_mark[row] = true;
    }
  }

  for ( auto row: Range(row_size()) ) {
    if ( !row_mark[row] ) {
      uncov_list.push_back(row);
    }
  }

  return uncov_list.empty();
}

// @brief 彩色結果が両立解になっているか調べる．
bool
ColCov::check_conflict(
  const vector<int>& color_map
)
{
  for ( auto p: mConflictList ) {
    auto col1 = p.first;
    auto col2 = p.second;
    auto c1 = color_map[col1];
    auto c2 = color_map[col2];
    if ( c1 != -1 && c2 != -1 && c1 == c2 ) {
      return false;
    }
  }
  return true;
}

END_NAMESPACE_DRUID
