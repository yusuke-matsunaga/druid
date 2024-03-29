#ifndef COLCOV_H
#define COLCOV_H

/// @file ColCov.h
/// @brief ColCov のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
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
  ColCov(
    SizeType row_size, ///< [in] 行数
    SizeType col_size  ///< [in] 列数
  );

  /// @brief デストラクタ
  ~ColCov();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを設定する．
  ///
  /// 以前の内容は破棄される．
  void
  resize(
    SizeType row_size, ///< [in] 行数
    SizeType col_size  ///< [in] 列数
  );

  /// @brief 行列の要素を追加する．
  void
  insert_elem(
    SizeType row_pos, ///< [in] 行の位置 ( 0 <= row_pos < row_size() )
    SizeType col_pos  ///< [in] 列の位置 ( 0 <= col_pos < col_size() )
  );

  /// @brief 列の衝突関係を追加する．
  void
  insert_conflict(
    SizeType col_pos1, ///< [in] 列1の位置 ( 0 <= col_pos1 < col_size() )
    SizeType col_pos2  ///< [in] 列2の位置 ( 0 <= col_pos2 < col_size() )
  );

  /// @brief 行数を得る．
  SizeType
  row_size() const
  {
    return mRowSize;
  }

  /// @brief 列数を得る．
  SizeType
  col_size() const
  {
    return mColSize;
  }

  /// @brief 行列の要素のリストを得る．
  const vector<pair<SizeType, SizeType>>&
  elem_list() const
  {
    return mElemList;
  }

  /// @brief 衝突関係のリストを得る．
  const vector<pair<SizeType, SizeType>>&
  conflict_list() const
  {
    return mConflictList;
  }

  /// @brief ヒューリスティックで解を求める．
  /// @return 色数を返す．
  ///
  /// * color_map は列番号をキーにして色番号を返す．
  /// * 使用していない列の番号は -1 となる．
  int
  heuristic(
    const string& algorithm, ///< [in] アルゴリズム名
    const string& option,    ///< [in] オプション
    vector<int>& color_map   ///< [out] 列の彩色結果
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 最初に MinCov を行いその後で coloring を行う．
  int
  mincov_color(
    const string& option,  ///< [in] オプション
    vector<int>& color_map ///< [out] 列の彩色結果
  );

  /// @brief 彩色結果が被覆解になっているか調べる．
  /// @retval true 被覆解になっている．
  /// @retval false 被覆されていない行がある．
  bool
  check_cover(
    const vector<int>& color_map, ///< [in] 解
    vector<int>& uncov_list	  ///< [out] 被覆されていない行番号のリスト
  );

  /// @brief 彩色結果が両立解になっているか調べる．
  /// @retval true 両立解になっている．
  /// @retval false 衝突している彩色結果があった．
  bool
  check_conflict(
    const vector<int>& color_map   ///< [in] 解
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 行数
  SizeType mRowSize;

  // 列数
  SizeType mColSize;

  // 行列の要素のリスト
  // (row_pos, col_pos) のリスト
  vector<pair<SizeType, SizeType>> mElemList;

  // 衝突関係のリスト
  // (col_pos1, col_pos2) のリスト
  vector<pair<SizeType, SizeType>> mConflictList;

};

END_NAMESPACE_DRUID

#endif // COLCOV_H
