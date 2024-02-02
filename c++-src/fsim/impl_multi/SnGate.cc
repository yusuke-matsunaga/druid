﻿
/// @file SnGate.cc
/// @brief SnGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
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
  SizeType level,
  const vector<SizeType>& inputs
) : SimNode{id, level},
    mFaninNum{inputs.size()},
    mFanins{new SizeType[mFaninNum]}
{
  // ファンインをセットする．
  for ( auto i: Range(0, mFaninNum) ) {
    mFanins[i] = inputs[i];
  }
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
SizeType
SnGate::fanin(
  SizeType pos
) const
{
  ASSERT_COND( pos >= 0 && pos < _fanin_num() );

  return _fanin(pos);
}

// @brief 内容をダンプする．
void
SnGate::dump(
  ostream& s
) const
{
  s <<  gate_type()
    << "(" << _fanin(0);
  for ( auto i: Range(1, _fanin_num()) ) {
    s << ", " << _fanin(i);
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate1 SimNode.h
// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate1::SnGate1(
  SizeType id,
  SizeType level,
  const vector<SizeType>& inputs
) : SimNode{id, level}
{
  ASSERT_COND( inputs.size() == 1 );

  mFanin = inputs[0];
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
SizeType
SnGate1::fanin(
  SizeType pos
) const
{
  return mFanin;
}

// @brief 内容をダンプする．
void
SnGate1::dump(
  ostream& s
) const
{
  s <<  gate_type()
    << "(" << _fanin() << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate2 SimNode.h
// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate2::SnGate2(
  SizeType id,
  SizeType level,
  const vector<SizeType>& inputs
) : SimNode{id, level}
{
  ASSERT_COND( inputs.size() == 2 );

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
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
SizeType
SnGate2::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate2::dump(
  ostream& s
) const
{
  s << gate_type()
    << "2(" << _fanin(0)
    << ", " << _fanin(1)
    << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate3 SimNode.h
// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate3::SnGate3(
  SizeType id,
  SizeType level,
  const vector<SizeType>& inputs
) : SimNode{id, level}
{
  ASSERT_COND( inputs.size() == 3 );

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
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
SizeType
SnGate3::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate3::dump(
  ostream& s
) const
{
  s << gate_type()
    << "3(" << _fanin(0)
    << ", " << _fanin(1)
    << ", " << _fanin(2)
    << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate4 SimNode.h
// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate4::SnGate4(
  SizeType id,
  SizeType level,
  const vector<SizeType>& inputs
) : SimNode{id, level}
{
  ASSERT_COND( inputs.size() == 4 );

  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  mFanins[3] = inputs[3];
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
SizeType
SnGate4::fanin(
  SizeType pos
) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate4::dump(
  ostream& s
) const
{
  s << gate_type()
    << "4(" << _fanin(0)
    << ", " << _fanin(1)
    << ", " << _fanin(2)
    << ", " << _fanin(3)
    << ")" << endl;
}

END_NAMESPACE_DRUID_FSIM
