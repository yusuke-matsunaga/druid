﻿
/// @file MpMatrix.cc
/// @brief MpMatrix の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MpMatrix.h"
#include "ym/Range.h"


//#define VERIFY_MCMATRIX 1


BEGIN_NAMESPACE_YM_SATPG

int mpmatrix_debug = 0;

//////////////////////////////////////////////////////////////////////
// クラス MpMatrix
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] row_size 行数
// @param[in] col_size 列数
// @param[in] cost_array コストの配列
// @param[in] elem_list 要素のリスト
MpMatrix::MpMatrix(int row_size,
		   int col_size,
		   const vector<int>& cost_array,
		   const vector<pair<int, int>>& elem_list) :
  mCellAlloc(sizeof(MpCell), 1024),
  mRowSize(0),
  mColSize(0),
  mRowArray(nullptr),
  mColArray(nullptr),
  mCostArray(nullptr),
  mDelStack(nullptr)
{
  resize(row_size, col_size);

  for ( auto row_col: elem_list ) {
    insert_elem(row_col.first, row_col.second);
  }
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のオブジェクト
MpMatrix::MpMatrix(const MpMatrix& src) :
  mCellAlloc(sizeof(MpCell), 1024),
  mRowSize(0),
  mColSize(0),
  mRowArray(nullptr),
  mColArray(nullptr),
  mCostArray(nullptr),
  mDelStack(nullptr)
{
  resize(src.row_size(), src.col_size());
  copy(src);
}

// @brief 代入演算子
// @param[in] src コピー元のオブジェクト
const MpMatrix&
MpMatrix::operator=(const MpMatrix& src)
{
  if ( this != &src ) {
    clear();
    resize(src.row_size(), src.col_size());
    copy(src);
  }
  return *this;
}

// @brief デストラクタ
MpMatrix::~MpMatrix()
{
  delete [] mRowArray;
  delete [] mColArray;
  delete [] mCostArray;
  delete [] mDelStack;
}

// @brief 内容をクリアする．
void
MpMatrix::clear()
{
  mCellAlloc.destroy();

  delete [] mRowArray;
  delete [] mColArray;
  delete [] mCostArray;
  delete [] mDelStack;

  mRowArray = nullptr;
  mColArray = nullptr;
  mCostArray = nullptr;
  mDelStack = nullptr;
}

// @brief サイズを変更する．
// @param[in] row_size 行数
// @param[in] col_size 列数
void
MpMatrix::resize(int row_size,
		 int col_size)
{
  if ( mRowSize != row_size || mColSize != col_size ) {
    clear();

    mRowSize = row_size;
    mColSize = col_size;

    mRowArray = new MpHead[mRowSize];
    for ( auto row_pos: Range(mRowSize) ) {
      mRowArray[row_pos].init(row_pos, false);
    }

    mColArray = new MpHead[mColSize];
    mCostArray = new int[mColSize];
    for ( auto col_pos: Range(mColSize) ) {
      mColArray[col_pos].init(col_pos, true);
      mCostArray[col_pos] = 1;
    }

    mDelStack = new MpHead*[row_size + col_size];
    mStackTop = 0;
  }
}

// @brief 内容をコピーする．
void
MpMatrix::copy(const MpMatrix& src)
{
  ASSERT_COND(row_size() == src.row_size() );
  ASSERT_COND(col_size() == src.col_size() );

  for ( auto row_pos: Range(row_size()) ) {
    for ( auto src_cell: row_head(row_pos)->row_list() ) {
      auto col_pos = src_cell->col_pos();
      insert_elem(row_pos, col_pos);
    }
  }

  for ( auto col_pos: Range(mColSize) ) {
    mCostArray[col_pos] = src.mCostArray[col_pos];
  }
}

// @brief 列集合のコストを返す．
// @param[in] col_list 列のリスト
int
MpMatrix::cost(const vector<int>& col_list) const
{
  int cur_cost = 0;
  for ( auto col: col_list ) {
    cur_cost += col_cost(col);
  }
  return cur_cost;
}

#if 0
// @brief ブロック分割を行う．
// @param[in] row_list1 1つめのブロックの行番号のリスト
// @param[in] row_list2 2つめのブロックの行番号のリスト
// @param[in] col_list1 1つめのブロックの列番号のリスト
// @param[in] col_list2 2つめのブロックの列番号のリスト
// @retval true ブロック分割が行われた．
// @retval false ブロック分割が行えなかった．
bool
MpMatrix::block_partition(vector<int>& row_list1,
			  vector<int>& row_list2,
			  vector<int>& col_list1,
			  vector<int>& col_list2) const
{
#if 0
  // マークを消す．
  for ( auto row1: row_list() ) {
    row1->mWork = 0U;
  }
  for ( auto col1: col_list() ) {
    col1->mWork = 0U;
  }

  // 最初の行から到達可能な行と列にマークをつける．
  auto row0 = row_list().front();
  int nc1 = mark_cols(row0);

  int nr = row_num();
  int nc = col_num();

  if ( nc == nc1 ) {
    return false;
  }

  ymuint bitmask = 0U;
  if ( nc1 > nc - nc1 ) {
    bitmask = 1U;
  }

  row_list1.clear();
  row_list1.reserve(nr);
  row_list2.clear();
  row_list2.reserve(nr);
  for ( auto row1: row_list() ) {
    if ( row1->mWork ^ bitmask ) {
      row_list1.push_back(row1->pos());
    }
    else {
      row_list2.push_back(row1->pos());
    }
  }

  col_list1.clear();
  col_list1.reserve(nc);
  col_list2.clear();
  col_list2.reserve(nc);
  for ( auto col1: col_list() ) {
    if ( col1->mWork ^ bitmask ) {
      col_list1.push_back(col1->pos());
    }
    else {
      col_list2.push_back(col1->pos());
    }
  }
#endif

  return true;
}
#endif

#if 0
// @brief col に接続している行をマークする．
// @param[in] col 対象の列
// @return マークされた列数を返す．
int
MpMatrix::mark_rows(const MpHead* col) const
{
  int nc = 0;
  for ( auto cell: col->col_list() ) {
    int row_pos = cell->row_pos();
    auto row1 = row(row_pos);
    if ( row1->mWork == 0U ) {
      row1->mWork = 1U;
      nc += mark_cols(row1);
    }
  }
  return nc;
}

// @brief row に接続している列をマークする．
// @param[in] row 対象の行
// @return マークされた列数を返す．
int
MpMatrix::mark_cols(const MpHead* row) const
{
  int nc = 0;
  for ( auto cell: row->row_list() ) {
    int col_pos = cell->col_pos();
    auto col1 = col(col_pos);
    if ( col1->mWork == 0U ) {
      col1->mWork = 1U;
      ++ nc;
      nc += mark_rows(col1);
    }
  }
  return nc;
}
#endif

// @brief 列集合がカバーになっているか検証する．
// @param[in] col_list 列のリスト
// @retval true col_list がカバーになっている．
// @retval false col_list でカバーされていない行がある．
bool
MpMatrix::verify(const vector<int>& col_list) const
{
  // カバーされた行の印
  vector<bool> row_mark(row_size(), false);

  // col_list の列でカバーされた行に印をつける．
  for ( auto col_pos: col_list ) {
    for ( auto cell: col_head(col_pos)->col_list() ) {
      auto row_pos = cell->row_pos();
      row_mark[row_pos] = true;
    }
  }

  // 印の付いていない行があったらエラー
  for ( auto row_pos: Range(row_size()) ) {
    if ( !row_mark[row_pos] ) {
      return false;
    }
  }
  return true;
}

// @brief 要素を追加する．
// @param[in] row_pos 追加する要素の行番号
// @param[in] col_pos 追加する要素の列番号
void
MpMatrix::insert_elem(int row_pos,
		      int col_pos)
{
  auto cell = alloc_cell(row_pos, col_pos);

  auto row_head1 = row_head(row_pos);
  bool stat1 = row_head1->row_insert(cell);
  if ( !stat1 ) {
    // 列番号が重複しているので無視する．
    free_cell(cell);
    return;
  }

  auto col_head1 = col_head(col_pos);
  bool stat2 = col_head1->col_insert(cell);
  ASSERT_COND( stat2 );
}

// @brief セルの生成
MpCell*
MpMatrix::alloc_cell(int row_pos,
		     int col_pos)
{
  void* p = mCellAlloc.get_memory(sizeof(MpCell));
  return new (p) MpCell(row_pos, col_pos);
}

// @brief セルの解放
void
MpMatrix::free_cell(MpCell* cell)
{
  mCellAlloc.put_memory(sizeof(MpCell), cell);
}

// @brief 内容を出力する．
// @param[in] s 出力先のストリーム
void
MpMatrix::print(ostream& s) const
{
  for ( auto col_pos: Range(col_size()) ) {
    if ( col_cost(col_pos) != 1 ) {
      s << "Col#" << col_pos << ": " << col_cost(col_pos) << endl;
    }
  }
  for ( auto row_pos: Range(row_size()) ) {
    s << "Row#" << row_pos << ":";
    for ( auto cell: row_head(row_pos)->row_list() ) {
      s << " " << cell->col_pos();
    }
    s << endl;
  }
}

END_NAMESPACE_YM_SATPG
