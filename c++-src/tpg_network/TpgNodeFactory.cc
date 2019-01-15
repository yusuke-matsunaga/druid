
/// @file TpgNodeFactory.cc
/// @brief TpgNodeFactory の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
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

// @brief コンストラクタ
// @param[in] alloc メモリアロケータ
TpgNodeFactory::TpgNodeFactory(Alloc& alloc) :
  mAlloc(alloc)
{
}

// @brief デストラクタ
TpgNodeFactory::~TpgNodeFactory()
{
}

// @brief 入力ノードを作る．
// @param[in] id ノード番号
// @param[in] iid 入力番号
// @param[in] fanout_num ファンアウト数
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_input(int id,
			   int iid,
			   int fanout_num)
{
  void* p = mAlloc.get_memory(sizeof(TpgInput));
  TpgNode* node = new (p) TpgInput(id, iid);
  node->set_fanout_num(fanout_num, mAlloc);

  return node;
}

// @brief 出力ノードを作る．
// @param[in] id ノード番号
// @param[in] oid 出力番号
// @param[in] inode 入力ノード
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_output(int id,
			    int oid,
			    TpgNode* inode)
{
  void* p = mAlloc.get_memory(sizeof(TpgOutput));
  TpgNode* node = new (p) TpgOutput(id, oid, inode);

  return node;
}

// @brief DFFの入力ノードを作る．
// @param[in] id ノード番号
// @param[in] oid 出力番号
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_dff_input(int id,
			       int oid,
			       const TpgDff* dff,
			       TpgNode* inode)
{
  void* p = mAlloc.get_memory(sizeof(TpgDffInput));
  TpgNode* node = new (p) TpgDffInput(id, oid, dff, inode);

  return node;
}

// @brief DFFの出力ノードを作る．
// @param[in] id ノード番号
// @param[in] iid 入力番号
// @param[in] dff 接続しているDFF
// @param[in] fanout_num ファンアウト数
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_dff_output(int id,
				int iid,
				const TpgDff* dff,
				int fanout_num)
{
  void* p = mAlloc.get_memory(sizeof(TpgDffOutput));
  TpgNode* node = new (p) TpgDffOutput(id, iid, dff);
  node->set_fanout_num(fanout_num, mAlloc);

  return node;
}

// @brief DFFのクロック端子を作る．
// @param[in] id ノード番号
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_dff_clock(int id,
			       const TpgDff* dff,
			       TpgNode* inode)
{
  void* p = mAlloc.get_memory(sizeof(TpgDffClock));
  TpgNode* node = new (p) TpgDffClock(id, dff, inode);

  return node;
}

// @brief DFFのクリア端子を作る．
// @param[in] id ノード番号
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_dff_clear(int id,
			       const TpgDff* dff,
			       TpgNode* inode)
{
  void* p = mAlloc.get_memory(sizeof(TpgDffClear));
  TpgNode* node = new (p) TpgDffClear(id, dff, inode);

  return node;
}

// @brief DFFのプリセット端子を作る．
// @param[in] id ノード番号
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_dff_preset(int id,
				const TpgDff* dff,
				TpgNode* inode)
{
  void* p = mAlloc.get_memory(sizeof(TpgDffPreset));
  TpgNode* node = new (p) TpgDffPreset(id, dff, inode);

  return node;
}

// @brief 論理ノードを作る．
// @param[in] id ノード番号
// @param[in] gate_type ゲートタイプ
// @param[in] inode_list 入力ノードのリスト
// @param[in] fanout_num ファンアウト数
// @return 作成したノードを返す．
TpgNode*
TpgNodeFactory::make_logic(int id,
			   GateType gate_type,
			   const vector<TpgNode*>& inode_list,
			   int fanout_num)
{
  void* p;
  int ni = inode_list.size();
  TpgNode* node = nullptr;
  switch ( gate_type ) {
  case GateType::Const0:
    ASSERT_COND( ni == 0 );

    p = mAlloc.get_memory(sizeof(TpgLogicC0));
    node = new (p) TpgLogicC0(id);
    break;

  case GateType::Const1:
    ASSERT_COND( ni == 0 );

    p = mAlloc.get_memory(sizeof(TpgLogicC1));
    node = new (p) TpgLogicC1(id);
    break;

  case GateType::Buff:
    ASSERT_COND( ni == 1 );

    p = mAlloc.get_memory(sizeof(TpgLogicBUFF));
    node = new (p) TpgLogicBUFF(id, inode_list[0]);
    break;

  case GateType::Not:
    ASSERT_COND( ni == 1 );

    p = mAlloc.get_memory(sizeof(TpgLogicNOT));
    node = new (p) TpgLogicNOT(id, inode_list[0]);
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      p = mAlloc.get_memory(sizeof(TpgLogicAND2));
      node = new (p) TpgLogicAND2(id, inode_list);
      break;

    case 3:
      p = mAlloc.get_memory(sizeof(TpgLogicAND3));
      node = new (p) TpgLogicAND3(id, inode_list);
      break;

    case 4:
      p = mAlloc.get_memory(sizeof(TpgLogicAND4));
      node = new (p) TpgLogicAND4(id, inode_list);
      break;

    default:
      p = mAlloc.get_memory(sizeof(TpgLogicANDN));
      node = new (p) TpgLogicANDN(id);
      node->set_fanin(inode_list, mAlloc);
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      p = mAlloc.get_memory(sizeof(TpgLogicNAND2));
      node = new (p) TpgLogicNAND2(id, inode_list);
      break;

    case 3:
      p = mAlloc.get_memory(sizeof(TpgLogicNAND3));
      node = new (p) TpgLogicNAND3(id, inode_list);
      break;

    case 4:
      p = mAlloc.get_memory(sizeof(TpgLogicNAND4));
      node = new (p) TpgLogicNAND4(id, inode_list);
      break;

    default:
      p = mAlloc.get_memory(sizeof(TpgLogicNANDN));
      node = new (p) TpgLogicNANDN(id);
      node->set_fanin(inode_list, mAlloc);
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      p = mAlloc.get_memory(sizeof(TpgLogicOR2));
      node = new (p) TpgLogicOR2(id, inode_list);
      break;

    case 3:
      p = mAlloc.get_memory(sizeof(TpgLogicOR3));
      node = new (p) TpgLogicOR3(id, inode_list);
      break;

    case 4:
      p = mAlloc.get_memory(sizeof(TpgLogicOR4));
      node = new (p) TpgLogicOR4(id, inode_list);
      break;

    default:
      p = mAlloc.get_memory(sizeof(TpgLogicORN));
      node = new (p) TpgLogicORN(id);
      node->set_fanin(inode_list, mAlloc);
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      p = mAlloc.get_memory(sizeof(TpgLogicNOR2));
      node = new (p) TpgLogicNOR2(id, inode_list);
      break;

    case 3:
      p = mAlloc.get_memory(sizeof(TpgLogicNOR3));
      node = new (p) TpgLogicNOR3(id, inode_list);
      break;

    case 4:
      p = mAlloc.get_memory(sizeof(TpgLogicNOR4));
      node = new (p) TpgLogicNOR4(id, inode_list);
      break;

    default:
      p = mAlloc.get_memory(sizeof(TpgLogicNORN));
      node = new (p) TpgLogicNORN(id);
      node->set_fanin(inode_list, mAlloc);
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );

    p = mAlloc.get_memory(sizeof(TpgLogicXOR2));
    node = new (p) TpgLogicXOR2(id, inode_list);
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );

    p = mAlloc.get_memory(sizeof(TpgLogicXNOR2));
    node = new (p) TpgLogicXNOR2(id, inode_list);
    break;

  default:
    ASSERT_NOT_REACHED;
  }
  node->set_fanout_num(fanout_num, mAlloc);

  return node;
}

END_NAMESPACE_DRUID
