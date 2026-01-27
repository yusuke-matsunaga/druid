
/// @file SnGate.cc
/// @brief SnGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
// @class SnGate SimNode.h
// @brief 多入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate::SnGate(
  SizeType id,
  const std::vector<SimNode*>& inputs
) : SimNode{id},
    mFaninNum{inputs.size()},
    mFanins{new SimNode*[mFaninNum]}
{
  // ファンインをセットしつつ
  // ファンインのレベルの最大値を求める．
  SizeType max_level = 0;
  for ( auto i: Range(0, mFaninNum) ) {
    auto input = inputs[i];
    mFanins[i] = input;
    SizeType level = input->level();
    max_level = std::max(max_level, level);
  }
  set_level(max_level + 1);
}

// @brief デストラクタ
SnGate::~SnGate()
{
  delete [] mFanins;
}

// @brief ファンイン数を得る．
SizeType
SnGate::fanin_num() const
{
  return _fanin_num();
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate::fanin(
  SizeType pos
) const
{
  if ( pos >= _fanin_num() ) {
    throw std::out_of_range{"pos is out of range"};
  }

  return _fanin(pos);
}

// @brief 内容をダンプする．
void
SnGate::dump(
  std::ostream& s
) const
{
  s <<  gate_type()
    << "(" << _fanin(0)->id();
  for ( auto i: Range(1, _fanin_num()) ) {
    s << ", " << _fanin(i)->id();
  }
  s << ")"
    << std::endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate1 SimNode.h
// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate1::SnGate1(
  SizeType id,
  const std::vector<SimNode*>& inputs
) : SimNode{id}
{
  if ( inputs.size() != 1 ) {
    throw std::invalid_argument{"inputs.size() != 1"};
  }

  mFanin = inputs[0];
  set_level(mFanin->level() + 1);
}

// @brief デストラクタ
SnGate1::~SnGate1()
{
}

// @brief ファンイン数を得る．
SizeType
SnGate1::fanin_num() const
{
  return 1;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate1::fanin(
  SizeType pos
) const
{
  return mFanin;
}

// @brief 内容をダンプする．
void
SnGate1::dump(
  std::ostream& s
) const
{
  s <<  gate_type()
    << "(" << _fanin()->id() << ")"
    << std::endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate2 SimNode.h
// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate2::SnGate2(
  SizeType id,
  const std::vector<SimNode*>& inputs
) : SimNode{id}
{
  if ( inputs.size() != 2 ) {
    throw std::invalid_argument{"inputs.size() != 2"};
  }

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  auto level = std::max(mFanins[0]->level(),
			mFanins[1]->level());
  set_level(level + 1);
}

// @brief デストラクタ
SnGate2::~SnGate2()
{
}

// @brief ファンイン数を得る．
SizeType
SnGate2::fanin_num() const
{
  return 2;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate2::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate2::dump(
  std::ostream& s
) const
{
  s << gate_type()
    << "2(" << _fanin(0)->id()
    << ", " << _fanin(1)->id()
    << ")"
    << std::endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate3 SimNode.h
// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate3::SnGate3(
  SizeType id,
  const std::vector<SimNode*>& inputs
) : SimNode{id}
{
  if ( inputs.size() != 3 ) {
    throw std::invalid_argument{"inputs.size() != 3"};
  }

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  auto level = mFanins[0]->level();
  level = std::max(level, mFanins[1]->level());
  level = std::max(level, mFanins[2]->level());
  set_level(level + 1);
}

// @brief デストラクタ
SnGate3::~SnGate3()
{
}

// @brief ファンイン数を得る．
SizeType
SnGate3::fanin_num() const
{
  return 3;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate3::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate3::dump(
  std::ostream& s
) const
{
  s << gate_type()
    << "3(" << _fanin(0)->id()
    << ", " << _fanin(1)->id()
    << ", " << _fanin(2)->id()
    << ")"
    << std::endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate4 SimNode.h
// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate4::SnGate4(
  SizeType id,
  const std::vector<SimNode*>& inputs
) : SimNode{id}
{
  if ( inputs.size() != 4 ) {
    throw std::invalid_argument{"inputs.size() != 4"};
  }

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  mFanins[3] = inputs[3];
  auto level = mFanins[0]->level();
  level = std::max(level, mFanins[1]->level());
  level = std::max(level, mFanins[2]->level());
  level = std::max(level, mFanins[3]->level());
  set_level(level + 1);
}

// @brief デストラクタ
SnGate4::~SnGate4()
{
}

// @brief ファンイン数を得る．
SizeType
SnGate4::fanin_num() const
{
  return 4;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate4::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate4::dump(
  std::ostream& s
) const
{
  s << gate_type()
    << "4(" << _fanin(0)->id()
    << ", " << _fanin(1)->id()
    << ", " << _fanin(2)->id()
    << ", " << _fanin(3)->id()
    << ")"
    << std::endl;
}

END_NAMESPACE_DRUID_FSIM
