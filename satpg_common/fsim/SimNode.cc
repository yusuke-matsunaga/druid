﻿
/// @file SimNode.cc
/// @brief SimNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "SimNode.h"
#include "SnInput.h"
#include "SnBuff.h"
#include "SnAnd.h"
#include "SnOr.h"
#include "SnXor.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のノードを表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
SimNode::SimNode(ymuint id) :
  mId(id),
  mFanoutNum(0),
  mFanoutTop(nullptr),
  mLevel(0)
{
}

// デストラクタ
SimNode::~SimNode()
{
  if ( fanout_num() > 1 ) {
    SimNode** fanouts = reinterpret_cast<SimNode**>(mFanoutTop);
    delete [] fanouts;
  }
}

// @brief 入力ノードを生成するクラスメソッド
SimNode*
SimNode::new_input(ymuint id)
{
  return new SnInput(id);
}

// @brief ゲートを生成するクラスメソッド
SimNode*
SimNode::new_gate(ymuint id,
		  GateType type,
		  const vector<SimNode*>& inputs)
{
  SimNode* node = nullptr;
  ymuint ni = inputs.size();
  switch ( type ) {
  case kGateBUFF:
    ASSERT_COND(ni == 1 );
    node = new SnBuff(id, inputs);
    break;

  case kGateNOT:
    ASSERT_COND(ni == 1 );
    node = new SnNot(id, inputs);
    break;

  case kGateAND:
    switch ( ni ) {
    case 2:  node = new SnAnd2(id, inputs); break;
    case 3:  node = new SnAnd3(id, inputs); break;
    case 4:  node = new SnAnd4(id, inputs); break;
    default: node = new SnAnd(id, inputs);  break;
    }
    break;

  case kGateNAND:
    switch ( ni ) {
    case 2:  node = new SnNand2(id, inputs); break;
    case 3:  node = new SnNand3(id, inputs); break;
    case 4:  node = new SnNand4(id, inputs); break;
    default: node = new SnNand(id, inputs);  break;
    }
    break;

  case kGateOR:
    switch ( ni ) {
    case 2:  node = new SnOr2(id, inputs); break;
    case 3:  node = new SnOr3(id, inputs); break;
    case 4:  node = new SnOr4(id, inputs); break;
    default: node = new SnOr(id, inputs);  break;
    }
    break;

  case kGateNOR:
    switch ( ni ) {
    case 2:  node = new SnNor2(id, inputs); break;
    case 3:  node = new SnNor3(id, inputs); break;
    case 4:  node = new SnNor4(id, inputs); break;
    default: node = new SnNor(id, inputs);  break;
    }
    break;

  case kGateXOR:
    switch ( ni ) {
    case 2:  node = new SnXor2(id, inputs); break;
    default: node = new SnXor(id, inputs);  break;
    }
    break;

  case kGateXNOR:
    switch ( ni ) {
    case 2:  node = new SnXnor2(id, inputs); break;
    default: node = new SnXnor(id, inputs);  break;
    }
    break;

  default:
    ASSERT_NOT_REACHED;
  }
  return node;
}

// @brief レベルを設定する．
void
SimNode::set_level(ymuint level)
{
  mLevel = level;
}

// @brief ファンアウトリストを作成する．
void
SimNode::set_fanout_list(const vector<SimNode*>& fo_list,
			 ymuint ipos)
{
  ymuint nfo = fo_list.size();
  if ( nfo > 0 ) {
    if ( nfo == 1 ) {
      mFanoutTop = fo_list[0];
    }
    else {
      SimNode** fanouts = new SimNode*[nfo];
      for (ymuint i = 0; i < nfo; ++ i) {
	fanouts[i] = fo_list[i];
      }
      mFanoutTop = reinterpret_cast<SimNode*>(fanouts);
    }
  }

  mFanoutNum |= (nfo << 16) | (ipos << 4);
}

END_NAMESPACE_YM_SATPG_FSIM
