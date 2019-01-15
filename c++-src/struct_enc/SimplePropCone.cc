
/// @file SimplePropCone.cc
/// @brief SimplePropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "SimplePropCone.h"
#include "StructEnc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] struct_sat StructEnc ソルバ
// @param[in] root_node FFRの根のノード
// @param[in] block_node ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
//
// ブロックノードより先のノードは含めない．
// 通常 block_node は nullptr か root_node の dominator
// となっているはず．
SimplePropCone::SimplePropCone(StructEnc& struct_sat,
			       const TpgNode* root_node,
			       const TpgNode* block_node,
			       bool detect) :
  PropCone(struct_sat, root_node, block_node, detect)
{
}

// @brief デストラクタ
SimplePropCone::~SimplePropCone()
{
}

// @brief 関係するノードの変数を作る．
void
SimplePropCone::make_vars()
{
  PropCone::make_vars();
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
SimplePropCone::make_cnf()
{
  PropCone::make_cnf();
}

// @brief 故障の影響伝搬させる条件を作る．
// @param[in] root 起点となるノード
// @param[out] assumptions 結果の仮定を表すリテラルのリスト
void
SimplePropCone::make_prop_condition(const TpgNode* root,
				    vector<SatLiteral>& assumptions)
{
}

END_NAMESPACE_DRUID_STRUCTENC
