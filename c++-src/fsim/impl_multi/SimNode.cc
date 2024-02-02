﻿
/// @file SimNode.cc
/// @brief SimNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "SimNode.h"
#include "SnInput.h"
#include "SnBuff.h"
#include "SnAnd.h"
#include "SnOr.h"
#include "SnXor.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のノードを表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
SimNode::SimNode(
  SizeType id,
  SizeType level
) : mId{id},
    mFlags{0},
    mFanoutNum{0},
    mFanoutTop{nullptr},
    mLevel{level}
{
}

// デストラクタ
SimNode::~SimNode()
{
  if ( fanout_num() > 1 ) {
    auto fanouts = reinterpret_cast<SimNode**>(mFanoutTop);
    delete [] fanouts;
  }
}

// @brief 入力ノードを生成するクラスメソッド
SimNode*
SimNode::new_input(
  SizeType id
)
{
  return new SnInput(id);
}

// @brief ゲートを生成するクラスメソッド
SimNode*
SimNode::new_gate(
  SizeType id,
  PrimType type,
  SizeType level,
  const vector<SizeType>& inputs
)
{
  SimNode* node = nullptr;
  auto ni = inputs.size();
  switch ( type ) {
  case PrimType::Buff:
    ASSERT_COND( ni == 1 );

    node = new SnBuff(id, level, inputs);
    break;

  case PrimType::Not:
    ASSERT_COND( ni == 1 );

    node = new SnNot(id, level, inputs);
    break;

  case PrimType::And:
    switch ( ni ) {
    case 2:  node = new SnAnd2(id, level, inputs); break;
    case 3:  node = new SnAnd3(id, level, inputs); break;
    case 4:  node = new SnAnd4(id, level, inputs); break;
    default: node = new SnAnd(id, level, inputs);  break;
    }
    break;

  case PrimType::Nand:
    switch ( ni ) {
    case 2:  node = new SnNand2(id, level, inputs); break;
    case 3:  node = new SnNand3(id, level, inputs); break;
    case 4:  node = new SnNand4(id, level, inputs); break;
    default: node = new SnNand(id, level, inputs);  break;
    }
    break;

  case PrimType::Or:
    switch ( ni ) {
    case 2:  node = new SnOr2(id, level, inputs); break;
    case 3:  node = new SnOr3(id, level, inputs); break;
    case 4:  node = new SnOr4(id, level, inputs); break;
    default: node = new SnOr(id, level, inputs);  break;
    }
    break;

  case PrimType::Nor:
    switch ( ni ) {
    case 2:  node = new SnNor2(id, level, inputs); break;
    case 3:  node = new SnNor3(id, level, inputs); break;
    case 4:  node = new SnNor4(id, level, inputs); break;
    default: node = new SnNor(id, level, inputs);  break;
    }
    break;

  case PrimType::Xor:
    switch ( ni ) {
    case 2:  node = new SnXor2(id, level, inputs); break;
    default: node = new SnXor(id, level, inputs);  break;
    }
    break;

  case PrimType::Xnor:
    switch ( ni ) {
    case 2:  node = new SnXnor2(id, level, inputs); break;
    default: node = new SnXnor(id, level, inputs);  break;
    }
    break;

  default:
    ASSERT_NOT_REACHED;
  }
  return node;
}

// @brief ファンアウトリストを作成する．
void
SimNode::set_fanout_list(
  const vector<SimNode*>& fo_list,
  SizeType ipos
)
{
  auto nfo = fo_list.size();
  if ( nfo > 0 ) {
    if ( nfo == 1 ) {
      mFanoutTop = fo_list[0];
    }
    else {
      SimNode** fanouts = new SimNode*[nfo];
      for ( auto i: Range(0, nfo) ) {
	fanouts[i] = fo_list[i];
      }
      mFanoutTop = reinterpret_cast<SimNode*>(fanouts);
    }
  }

  mFanoutNum = (nfo << 8) | ipos;
}

END_NAMESPACE_DRUID_FSIM
