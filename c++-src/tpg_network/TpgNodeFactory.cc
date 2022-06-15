
/// @file TpgNodeFactory.cc
/// @brief TpgNodeFactory の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNodeFactory.h"

#include "TpgInput.h"
#include "TpgOutput.h"

#include "TpgDffInput.h"
#include "TpgDffOutput.h"
#include "TpgDffClock.h"
#include "TpgDffClear.h"
#include "TpgDffPreset.h"

#include "TpgLogicC0.h"
#include "TpgLogicC1.h"
#include "TpgLogicBUFF.h"
#include "TpgLogicNOT.h"
#include "TpgLogicAND.h"
#include "TpgLogicNAND.h"
#include "TpgLogicOR.h"
#include "TpgLogicNOR.h"
#include "TpgLogicXOR.h"
#include "TpgLogicXNOR.h"

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgNodeFactory
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを作る．
TpgNode*
TpgNodeFactory::make_input(
  int id,
  int iid,
  int fanout_num
)
{
  TpgNode* node = new TpgInput(id, iid);
  node->set_fanout_num(fanout_num);

  return node;
}

// @brief 出力ノードを作る．
TpgNode*
TpgNodeFactory::make_output(
  int id,
  int oid,
  TpgNode* inode
)
{
  TpgNode* node = new TpgOutput(id, oid, inode);

  return node;
}

// @brief DFFの入力ノードを作る．
TpgNode*
TpgNodeFactory::make_dff_input(
  int id,
  int oid,
  const TpgDff* dff,
  TpgNode* inode
)
{
  TpgNode* node = new TpgDffInput(id, oid, dff, inode);

  return node;
}

// @brief DFFの出力ノードを作る．
TpgNode*
TpgNodeFactory::make_dff_output(
  int id,
  int iid,
  const TpgDff* dff,
  int fanout_num
)
{
  TpgNode* node = new TpgDffOutput(id, iid, dff);
  node->set_fanout_num(fanout_num);

  return node;
}

// @brief DFFのクロック端子を作る．
TpgNode*
TpgNodeFactory::make_dff_clock(
  int id,
  const TpgDff* dff,
  TpgNode* inode
)
{
  TpgNode* node = new TpgDffClock(id, dff, inode);

  return node;
}

// @brief DFFのクリア端子を作る．
TpgNode*
TpgNodeFactory::make_dff_clear(
  int id,
  const TpgDff* dff,
  TpgNode* inode
)
{
  TpgNode* node = new TpgDffClear(id, dff, inode);

  return node;
}

// @brief DFFのプリセット端子を作る．
TpgNode*
TpgNodeFactory::make_dff_preset(
  int id,
  const TpgDff* dff,
  TpgNode* inode
)
{
  TpgNode* node = new TpgDffPreset(id, dff, inode);

  return node;
}

// @brief 論理ノードを作る．
TpgNode*
TpgNodeFactory::make_logic(
  int id,
  GateType gate_type,
  const vector<TpgNode*>& inode_list,
  int fanout_num
)
{
  int ni = inode_list.size();
  TpgNode* node = nullptr;
  switch ( gate_type ) {
  case GateType::Const0:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC0(id);
    break;

  case GateType::Const1:
    ASSERT_COND( ni == 0 );

    node = new TpgLogicC1(id);
    break;

  case GateType::Buff:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicBUFF(id, inode_list[0]);
    break;

  case GateType::Not:
    ASSERT_COND( ni == 1 );

    node = new TpgLogicNOT(id, inode_list[0]);
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      node = new TpgLogicAND2(id, inode_list);
      break;

    case 3:
      node = new TpgLogicAND3(id, inode_list);
      break;

    case 4:
      node = new TpgLogicAND4(id, inode_list);
      break;

    default:
      node = new TpgLogicANDN(id);
      node->set_fanin(inode_list);
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      node = new TpgLogicNAND2(id, inode_list);
      break;

    case 3:
      node = new TpgLogicNAND3(id, inode_list);
      break;

    case 4:
      node = new TpgLogicNAND4(id, inode_list);
      break;

    default:
      node = new TpgLogicNANDN(id);
      node->set_fanin(inode_list);
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      node = new TpgLogicOR2(id, inode_list);
      break;

    case 3:
      node = new TpgLogicOR3(id, inode_list);
      break;

    case 4:
      node = new TpgLogicOR4(id, inode_list);
      break;

    default:
      node = new TpgLogicORN(id);
      node->set_fanin(inode_list);
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      node = new TpgLogicNOR2(id, inode_list);
      break;

    case 3:
      node = new TpgLogicNOR3(id, inode_list);
      break;

    case 4:
      node = new TpgLogicNOR4(id, inode_list);
      break;

    default:
      node = new TpgLogicNORN(id);
      node->set_fanin(inode_list);
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXOR2(id, inode_list);
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );

    node = new TpgLogicXNOR2(id, inode_list);
    break;

  default:
    ASSERT_NOT_REACHED;
  }
  node->set_fanout_num(fanout_num);

  return node;
}

END_NAMESPACE_DRUID
