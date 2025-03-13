
/// @file AigGen.cc
/// @brief AigGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "AigGen.h"
#include "Expr2Aig.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス AigGen_Naive
//////////////////////////////////////////////////////////////////////

// @brief Expr のリストから CNF を作る．
std::vector<AigHandle>
AigGen_Naive::conv(
  AigMgr& mgr,
  const vector<Expr>& expr_list
)
{
  Expr2Aig expr2aig(mgr);
  return expr2aig.conv_to_aig(expr_list);
}


//////////////////////////////////////////////////////////////////////
// クラス AigGen_Aig
//////////////////////////////////////////////////////////////////////


END_NAMESPACE_DRUID
