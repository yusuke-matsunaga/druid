
/// @file BgNode.cc
/// @brief BgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BgNode.h"
#include "BgNode_int.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス BgNode
//////////////////////////////////////////////////////////////////////

// @brief PLIT タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_PLIT(
  SizeType varid
)
{
  return new BgNode_PLIT{varid};
}

// @brief NLIT タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_NLIT(
  SizeType varid
)
{
  return new BgNode_NLIT{varid};
}

// @brief AND タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_AND(
  const vector<const BgNode*>& operand_list,
  const BgNode* cofactor
)
{
  return new BgNode_AND{operand_list, cofactor};
}

// @brief OR タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_OR(
  const vector<const BgNode*>& operand_list,
  const BgNode* cofactor
)
{
  return new BgNode_OR{operand_list, cofactor};
}

// @brief DEC0 タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_DEC0(
  SizeType varid,
  const BgNode* cofactor0,
  const BgNode* cofactor1
)
{
  return new BgNode_DEC0{varid, cofactor0, cofactor1};
}

// @brief DEC0 タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_DEC1(
  SizeType varid,
  const BgNode* cofactor0,
  const BgNode* cofactor1
)
{
  return new BgNode_DEC1{varid, cofactor0, cofactor1};
}

// @brief DEC0 タイプのノードを作るクラスメソッド
const BgNode*
BgNode::new_DEC2(
  SizeType varid,
  const BgNode* cofactor0,
  const BgNode* cofactor1
)
{
  return new BgNode_DEC2{varid, cofactor0, cofactor1};
}

// @brief PLIT 型の時に true を返す．
bool
BgNode::is_PLIT() const
{
  return false;
}

// @brief NLIT 型の時に true を返す．
bool
BgNode::is_NLIT() const
{
  return false;
}

// @brief AND 型の時に true を返す．
bool
BgNode::is_AND() const
{
  return false;
}

// @brief OR 型の時に true を返す．
bool
BgNode::is_OR() const
{
  return false;
}

// @brief DEC0 型の時に true を返す．
bool
BgNode::is_DEC0() const
{
  return false;
}

/// @brief DEC1 型の時に true を返す．
bool
BgNode::is_DEC1() const
{
  return false;
}

// @brief DEC2 型の時に true を返す．
bool
BgNode::is_DEC2() const
{
  return false;
}

// @brief 変数番号を返す．
SizeType
BgNode::varid() const
{
  throw std::invalid_argument{"Not a valid type for 'varid'"};
  return 0;
}

// @brief オペランドの数を返す．
SizeType
BgNode::operand_num() const
{
  throw std::invalid_argument{"Not a valid type for 'operand_num'"};
  return 0;
}

// @brief オペランドを返す．
const BgNode*
BgNode::operand(
  SizeType pos
) const
{
  throw std::invalid_argument{"Not a valid type for 'operand'"};
  return nullptr;
}

// @brief オペランドのリストを返す．
vector<const BgNode*>
BgNode::operand_list() const
{
  throw std::invalid_argument{"Not a valid type for 'operand_list'"};
  return {};
}

// @brief コファクターノードを返す．
const BgNode*
BgNode::cofactor() const
{
  throw std::invalid_argument{"Not a valid type for 'cofactor'"};
  return nullptr;
}

// @brief 負のコファクターを返す．
const BgNode*
BgNode::cofacotr0() const
{
  throw std::invalid_argument{"Not a valid type for 'cofactor0'"};
  return nullptr;
}

// @brief 正のコファクターを返す．
const BgNode*
BgNode::cofacotr1() const
{
  throw std::invalid_argument{"Not a valid type for 'cofactor1'"};
  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_LIT
//////////////////////////////////////////////////////////////////////

// @brief 変数番号を返す．
SizeType
BgNode_LIT::varid() const
{
  return mVarId;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_LIT::cnf_size() const
{
  return CnfSize::zero();
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_PLIT
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_PLIT::type() const
{
  return BgNode::PLIT;
}

// @brief PLIT 型の時に true を返す．
bool
BgNode_PLIT::is_PLIT() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_NLIT
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_NLIT::type() const
{
  return BgNode::NLIT;
}

// @brief NLIT 型の時に true を返す．
bool
BgNode_NLIT::is_NLIT() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_OP
//////////////////////////////////////////////////////////////////////

// @brief デストラクタ
BgNode_OP::~BgNode_OP()
{
  for ( auto node: mOperandList ) {
    delete node;
  }
}

// @brief オペランドの数を返す．
SizeType
BgNode_OP::operand_num() const
{
  return mOperandList.size();
}

// @brief オペランドを返す．
const BgNode*
BgNode_OP::operand(
  SizeType pos
) const
{
  if ( pos >= operand_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }
  return mOperandList[pos];
}

// @brief オペランドのリストを返す．
vector<const BgNode*>
BgNode_OP::operand_list() const
{
  return mOperandList;
}

// @brief コファクターを返す．
const BgNode*
BgNode_OP::cofactor() const
{
  return mCofactor;
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_AND
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_AND::type() const
{
  return BgNode::AND;
}

// @brief AND 型の時に true を返す．
bool
BgNode_AND::is_AND() const
{
  return true;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_AND::cnf_size() const
{
  auto n = operand_num();
  return CnfSize{n, n * 2};
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_OR
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_OR::type() const
{
  return BgNode::OR;
}

// @brief OR 型の時に true を返す．
bool
BgNode_OR::is_OR() const
{
  return true;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_OR::cnf_size() const
{
  auto n = operand_num();
  return CnfSize{1, n + 1};
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_DEC
//////////////////////////////////////////////////////////////////////

// @brief 負のコファクターを返す．
const BgNode*
BgNode_DEC::cofacotr0() const
{
  return mCofactor0;
}

// @brief 正のコファクターを返す．
const BgNode*
BgNode_DEC::cofacotr1() const
{
  return mCofactor1;
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_DEC0
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_DEC0::type() const
{
  return BgNode::DEC0;
}

// @brief DEC0 型の時に true を返す．
bool
BgNode_DEC0::is_DEC0() const
{
  return true;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_DEC0::cnf_size() const
{
  return CnfSize{2, 5};
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_DEC1
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_DEC1::type() const
{
  return BgNode::DEC1;
}

// @brief DEC1 型の時に true を返す．
bool
BgNode_DEC1::is_DEC1() const
{
  return true;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_DEC1::cnf_size() const
{
  return CnfSize{2, 5};
}


//////////////////////////////////////////////////////////////////////
// クラス BgNode_DEC2
//////////////////////////////////////////////////////////////////////

// @brief 種類を返す．
BgNode::Type
BgNode_DEC2::type() const
{
  return BgNode::DEC2;
}

// @brief DEC2 型の時に true を返す．
bool
BgNode_DEC2::is_DEC2() const
{
  return true;
}

// @brief このノードを CNF に変換する際のサイズを返す．
CnfSize
BgNode_DEC2::cnf_size() const
{
  return CnfSize{2, 6};
}

END_NAMESPACE_DRUID
