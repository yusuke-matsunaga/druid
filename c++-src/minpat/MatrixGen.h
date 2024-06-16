#ifndef MATRIXGEN_H
#define MATRIXGEN_H

/// @file MatrixGen.h
/// @brief MatrixGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/McMatrix.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MatrixGen MatrixGen.h "MatrixGen.h"
/// @brief 被覆行列を作るクラス．
///
/// 対象の故障リストとテストベクタのリストを与えて被覆行列を作る．
/// 被覆行列は故障を行に，テストベクタを列に対応させた行列で，
/// テストベクタが故障を検出する場合に１の要素を持つ．
//////////////////////////////////////////////////////////////////////
class MatrixGen
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 被覆行列を作る．
  static
  McMatrix
  generate(
    const TpgNetwork& network,		       ///< [in] ネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 故障のリスト
    const vector<TestVector>& tv_list	       ///< [in] テストパタンのリスト
  );

};

END_NAMESPACE_DRUID

#endif // MATRIXGEN_H
