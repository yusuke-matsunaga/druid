
/// @file BackTracer.cc
/// @brief BackTracer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "sa/BackTracer.h"
#include "BtSimple.h"
#include "BtJust1.h"
#include "BtJust2.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
// クラス BackTracer
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ID番号の最大値
BackTracer::BackTracer(ymuint xmode,
		       ymuint max_id)
{
  switch ( xmode ) {
  case 0: mImpl = new BtSimple(); break;
  case 1: mImpl = new BtJust1(); break;
  case 2: mImpl = new BtJust2(); break;
  default: mImpl = new BtJust2(); break;
  }
  mImpl->set_max_id(max_id);
}

// @brief デストラクタ
BackTracer::~BackTracer()
{
  delete mImpl;
}

// @brief バックトレースを行なう．
// @param[in] fnode 故障のあるノード
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
BackTracer::operator()(const TpgNode* fnode,
		       const NodeValList& assign_list,
		       const vector<const TpgNode*>& output_list,
		       const ValMap& val_map,
		       NodeValList& pi_assign_list)
{
  mImpl->run(fnode, assign_list, output_list, val_map, pi_assign_list);
}

END_NAMESPACE_YM_SATPG_SA
