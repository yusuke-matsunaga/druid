#ifndef COLCOV_H
#define COLCOV_H

/// @file ColCov.h
/// @brief ColCov のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ColCov ColCov.h "ColCov.h"
/// @brief color covering 問題を表すクラス
//////////////////////////////////////////////////////////////////////
class ColCov
{
public:

  /// @brief コンストラクタ
  ColCov();

  /// @brief コンストラクタ
  /// @param[in] row_size 行数
  /// @param[in] col_size 列数
  ColCov(int row_size,
	 int col_size);

  /// @brief デストラクタ
  ~ColCov();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを設定する．
  /// @param[in] row_size 行数
  /// @param[in] col_size 列数
  ///
  /// 以前の内容は破棄される．
  void
  resize(int row_size,
	 int col_size);

  /// @brief 行列の要素を追加する．
  /// @param[in] row_pos 行の位置 ( 0 <= row_pos < row_size() )
  /// @param[in] col_pos 列の位置 ( 0 <= col_pos < col_size() )
  void
  insert_elem(int row_pos,
	      int col_pos);

  /// @brief 列の衝突関係を追加する．
  /// @param[in] col_pos1, col_pos2 列の位置 ( 0 <= col_pos1, col_pos2 < col_size() )
  void
  insert_conflict(int col_pos1,
		  int col_pos2);

  /// @brief 行数を得る．
  int
  row_size() const;

  /// @brief 列数を得る．
  int
  col_size() const;

  /// @brief 行列の要素のリストを得る．
  const vector<pair<int, int>>&
  elem_list() const;

  /// @brief 衝突関係のリストを得る．
  const vector<pair<int, int>>&
  conflict_list() const;

  /// @brief ヒューリスティックで解を求める．
  /// @param[in] algorithm アルゴリズム名
  /// @param[in] option オプション
  /// @param[out] color_map 列の彩色結果
  /// @return 色数を返す．
  ///
  /// * color_map は列番号をキーにして色番号を返す．
  /// * 使用していない列の番号は -1 となる．
  int
  heuristic(const string& algorithm,
	    const string& option,
	    vector<int>& color_map);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 最初に MinCov を行いその後で coloring を行う．
  /// @param[in] option オプション
  /// @param[out] color_map 列の彩色結果
  int
  mincov_color(const string& option,
	       vector<int>& color_map);

  /// @brief 彩色結果が被覆解になっているか調べる．
  /// @param[in] color_map 解
  /// @param[out] uncov_list 被覆されていない行番号のリスト
  /// @retval true 被覆解になっている．
  /// @retval false 被覆されていない行がある．
  bool
  check_cover(const vector<int>& color_map,
	      vector<int>& uncov_list);

  /// @brief 彩色結果が両立解になっているか調べる．
  /// @param[in] color_map 解
  /// @retval true 両立解になっている．
  /// @retval false 衝突している彩色結果があった．
  bool
  check_conflict(const vector<int>& color_map);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 行数
  int mRowSize;

  // 列数
  int mColSize;

  // 行列の要素のリスト
  // (row_pos, col_pos) のリスト
  vector<pair<int, int>> mElemList;

  // 衝突関係のリスト
  // (col_pos1, col_pos2) のリスト
  vector<pair<int, int>> mConflictList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 行数を得る．
inline
int
ColCov::row_size() const
{
  return mRowSize;
}

// @brief 列数を得る．
inline
int
ColCov::col_size() const
{
  return mColSize;
}

// @brief 行列の要素のリストを得る．
inline
const vector<pair<int, int>>&
ColCov::elem_list() const
{
  return mElemList;
}

// @brief 衝突関係のリストを得る．
inline
const vector<pair<int, int>>&
ColCov::conflict_list() const
{
  return mConflictList;
}

END_NAMESPACE_DRUID

#endif // COLCOV_H
