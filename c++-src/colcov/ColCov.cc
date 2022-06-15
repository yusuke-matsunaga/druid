
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
  mRowSize(0),
  mColSize(0)
{
}

// @brief コンストラクタ
// @param[in] row_size 行数
// @param[in] col_size 列数
ColCov::ColCov(int row_size,
	       int col_size) :
  mRowSize(row_size),
  mColSize(col_size)
{
}

// @brief デストラクタ
ColCov::~ColCov()
{
}

// @brief サイズを設定する．
// @param[in] row_size 行数
// @param[in] col_size 列数
//
// 以前の内容は破棄される．
void
ColCov::resize(int row_size,
	       int col_size)
{
  mRowSize = row_size;
  mColSize = col_size;

  mElemList.clear();
  mConflictList.clear();
}

// @brief 行列の要素を追加する．
// @param[in] row_pos 行の位置
// @param[in] col_pos 列の位置
void
ColCov::insert_elem(int row_pos,
		    int col_pos)
{
  ASSERT_COND( row_pos >= 0 && row_pos < row_size() );
  ASSERT_COND( col_pos >= 0 && col_pos < col_size() );

  mElemList.push_back(make_pair(row_pos, col_pos));
}

// @brief 列の衝突関係を追加する．
// @param[in] col_pos1, col_pos2 列の位置
void
ColCov::insert_conflict(int col_pos1,
			int col_pos2)
{
  ASSERT_COND( col_pos1 >= 0 && col_pos1 < col_size() );
  ASSERT_COND( col_pos2 >= 0 && col_pos2 < col_size() );

  if ( col_pos1 > col_pos2 ) {
    // col_pos1 < col_pos2 になるように正規化する．
    int tmp = col_pos1;
    col_pos1 = col_pos2;
    col_pos2 = tmp;
  }
  mConflictList.push_back(make_pair(col_pos1, col_pos2));
}

// @brief ヒューリスティックで解を求める．
// @param[in] algorithm アルゴリズム名
// @param[in] option オプション
// @param[out] color_map 列の彩色結果
// @return 色数を返す．
//
// * color_map は列番号をキーにして色番号を返す．
// * 使用していない列の番号は -1 となる．
int
ColCov::heuristic(const string& algorithm,
		  const string& option,
		  vector<int>& color_map)
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
// @param[in] option オプション
int
ColCov::mincov_color(const string& option,
		     vector<int>& color_map)
{
  MinCov mincov(row_size(), col_size());
  for ( auto p: mElemList ) {
    mincov.insert_elem(p.first, p.second);
  }

  // 列を選ぶ．
  vector<int> sel_cols;
  mincov.heuristic(sel_cols);

  // 選ばれた列を前に詰める．
  int new_col_size = sel_cols.size();
  vector<int> col_map(col_size(), -1);
  vector<int> rcol_map(new_col_size);
  for ( auto new_col: Range(new_col_size) ) {
    auto col = sel_cols[new_col];
    col_map[col] = new_col;
    rcol_map[new_col] = col;
  }


  int ne = 0;
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

  vector<int> _color_map;
  int col_num = graph.coloring(_color_map, "isx");
  color_map.clear();
  color_map.resize(col_size(), -1);
  for ( auto new_col: Range(new_col_size) ) {
    auto col = rcol_map[new_col];
    color_map[col] = _color_map[new_col];
  }

  return col_num;
}

// @brief 被覆解になっているか調べる．
// @param[in] color_map 解
// @param[out] uncov_list 被覆されていない行番号のリスト
// @retval true 被覆解になっている．
// @retval false 被覆されていない行がある．
bool
ColCov::check_cover(const vector<int>& color_map,
		    vector<int>& uncov_list)
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
// @param[in] color_map 解
// @retval true 両立解になっている．
// @retval false 衝突している彩色結果があった．
bool
ColCov::check_conflict(const vector<int>& color_map)
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
