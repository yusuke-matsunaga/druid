
/// @file DtpgEngine_Node.cc
/// @brief DtpgEngine_Node の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine_Node.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DtpgEngine_Node::DtpgEngine_Node(
  const TpgNetwork& network,
  const TpgNode* node,
  const JsonValue& option
) : DtpgEngine{network, node, option}
{
}

// @brief デストラクタ
DtpgEngine_Node::~DtpgEngine_Node()
{
}

// @brief 故障伝搬の起点ノードを返す．
const TpgNode*
DtpgEngine_Node::fault_origin(
  const TpgFault* fault
)
{
  return fault->origin_node();
}

// @brief gen_pattern() で用いる検出条件を作る．
NodeTimeValList
DtpgEngine_Node::fault_condition(
  const TpgFault* fault
)
{
  return fault->excitation_condition();
}

END_NAMESPACE_DRUID
