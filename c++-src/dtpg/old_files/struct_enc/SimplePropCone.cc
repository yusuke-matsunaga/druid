
/// @file SimplePropCone.cc
/// @brief SimplePropCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "SimplePropCone.h"
#include "StructEnc.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

bool debug = false;

END_NONAMESPACE

// @brief コンストラクタ
SimplePropCone::SimplePropCone(
  StructEnc& struct_enc,
  const TpgNode* root_node
) : PropCone{struct_enc, root_node}
{
}

// @brief デストラクタ
SimplePropCone::~SimplePropCone()
{
}

// @brief 関係するノードの変数を作る．
void
SimplePropCone::make_vars(
  const vector<const TpgNode*>& node_list
)
{
  PropCone::make_vars(node_list);
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
SimplePropCone::make_cnf()
{
  PropCone::make_cnf();
}

// @brief 故障の影響伝搬させる条件を作る．
vector<SatLiteral>
SimplePropCone::make_condition(
  const TpgNode* root
)
{
  return {};
}

END_NAMESPACE_DRUID
