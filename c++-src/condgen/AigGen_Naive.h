#ifndef AIGGEN_NAIVE_H
#define AIGGEN_NAIVE_H

/// @file AigGen_Naive.h
/// @brief AigGen_Naive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "AigGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AigGen_Naive AigGen_Naive.h "AigGen_Naive.h"
/// @brief 式をそのまま AIG に変換する AigGen
//////////////////////////////////////////////////////////////////////
class AigGen_Naive :
  public AigGen
{
public:

  /// @brief コンストラクタ
  AigGen_Naive() = default;

  /// @brief デストラクタ
  ~AigGen_Naive() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // AigGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr のリストから AIG を作る．
  vector<AigHandle>
  conv(
    AigMgr& mgr,                  ///< [in] AIGマネージャ
    const vector<Expr>& expr_list ///< [in] 式のリスト
  ) override;

};

END_NAMESPACE_DRUID

#endif // AIGGEN_NAIVE_H
