﻿
/// @file SnOr.cc
/// @brief SnOr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SnOr.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

//////////////////////////////////////////////////////////////////////
// SnOr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr::SnOr(ymuint32 id,
	   const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnOr::~SnOr()
{
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnOr::_calc_hval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->hval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->hval();
  }
  return new_val;
}

// @brief 正常値の計算を行う．
PackedVal
SnOr::_calc_gval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->gval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->gval();
  }
  return new_val;
}

// @brief ゲートタイプを返す．
GateType
SnOr::gate_type() const
{
  return kGateOR;
}

// @brief 故障値の計算を行う．
PackedVal
SnOr::_calc_fval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->fval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->fval();
  }
  return new_val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr::calc_gobs(ymuint ipos)
{
  PackedVal obs = kPvAll0;
  for (ymuint i = 0; i < ipos; ++ i) {
    obs |= mFanins[i]->gval();
  }
  for (ymuint i = ipos + 1; i < mNfi; ++ i) {
    obs |= mFanins[i]->gval();
  }
  return ~obs;
}

// @brief 内容をダンプする．
void
SnOr::dump(ostream& s) const
{
  ymuint n = mNfi;
  s << "OR(" << mFanins[0]->id();
  for (ymuint i = 1; i < n; ++ i) {
    s << ", " << mFanins[i]->id();
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnOr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr2::SnOr2(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnOr2::~SnOr2()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr2::gate_type() const
{
  return kGateOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnOr2::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  return pat0 | pat1;
}

// @brief 正常値の計算を行う．
PackedVal
SnOr2::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  return pat0 | pat1;
}

// @brief 故障値の計算を行う．
PackedVal
SnOr2::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  return pat0 | pat1;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr2::calc_gobs(ymuint ipos)
{
  return ~mFanins[ipos ^ 1]->gval();
}

// @brief 内容をダンプする．
void
SnOr2::dump(ostream& s) const
{
  s << "OR2(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnOr3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr3::SnOr3(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnGate3(id, inputs)
{
}

// @brief デストラクタ
SnOr3::~SnOr3()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr3::gate_type() const
{
  return kGateOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnOr3::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  PackedVal pat2 = mFanins[2]->hval();
  return pat0 | pat1 | pat2;
}

// @brief 正常値の計算を行う．
PackedVal
SnOr3::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  PackedVal pat2 = mFanins[2]->gval();
  return pat0 | pat1 | pat2;
}

// @brief 故障値の計算を行う．
PackedVal
SnOr3::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  PackedVal pat2 = mFanins[2]->fval();
  return pat0 | pat1 | pat2;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr3::calc_gobs(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return ~(mFanins[1]->gval() | mFanins[2]->gval());
  case 1: return ~(mFanins[0]->gval() | mFanins[2]->gval());
  case 2: return ~(mFanins[0]->gval() | mFanins[1]->gval());
  }
  return kPvAll0;
}

// @brief 内容をダンプする．
void
SnOr3::dump(ostream& s) const
{
  s << "OR3(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ", " << mFanins[2]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnOr4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr4::SnOr4(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnGate4(id, inputs)
{
}

// @brief デストラクタ
SnOr4::~SnOr4()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr4::gate_type() const
{
  return kGateOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnOr4::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  PackedVal pat2 = mFanins[2]->hval();
  PackedVal pat3 = mFanins[3]->hval();
  return pat0 | pat1 | pat2 | pat3;
}

// @brief 正常値の計算を行う．
PackedVal
SnOr4::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  PackedVal pat2 = mFanins[2]->gval();
  PackedVal pat3 = mFanins[3]->gval();
  return pat0 | pat1 | pat2 | pat3;
}

// @brief 故障値の計算を行う．
PackedVal
SnOr4::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  PackedVal pat2 = mFanins[2]->fval();
  PackedVal pat3 = mFanins[3]->fval();
  return pat0 | pat1 | pat2 | pat3;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr4::calc_gobs(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return ~(mFanins[1]->gval() | mFanins[2]->gval() | mFanins[3]->gval());
  case 1: return ~(mFanins[0]->gval() | mFanins[2]->gval() | mFanins[3]->gval());
  case 2: return ~(mFanins[0]->gval() | mFanins[1]->gval() | mFanins[3]->gval());
  case 3: return ~(mFanins[0]->gval() | mFanins[1]->gval() | mFanins[2]->gval());
  }
  return kPvAll0;
}

// @brief 内容をダンプする．
void
SnOr4::dump(ostream& s) const
{
  s << "OR4(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ", " << mFanins[2]->id();
  s << ", " << mFanins[3]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnNor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor::SnNor(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnOr(id, inputs)
{
}

// @brief デストラクタ
SnNor::~SnNor()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor::gate_type() const
{
  return kGateNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnNor::_calc_hval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->hval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->hval();
  }
  return ~new_val;
}

// @brief 正常値の計算を行う．
PackedVal
SnNor::_calc_gval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->gval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->gval();
  }
  return ~new_val;
}

// @brief 故障値の計算を行う．
PackedVal
SnNor::_calc_fval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->fval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val |= mFanins[i]->fval();
  }
  return ~new_val;
}

// @brief 内容をダンプする．
void
SnNor::dump(ostream& s) const
{
  ymuint n = mNfi;
  s << "NOR(" << mFanins[0]->id();
  for (ymuint i = 1; i < n; ++ i) {
    s << ", " << mFanins[i]->id();
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnNor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor2::SnNor2(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnOr2(id, inputs)
{
}

// @brief デストラクタ
SnNor2::~SnNor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor2::gate_type() const
{
  return kGateNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnNor2::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  return ~(pat0 | pat1);
}

// @brief 正常値の計算を行う．
PackedVal
SnNor2::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  return ~(pat0 | pat1);
}

// @brief 故障値の計算を行う．
PackedVal
SnNor2::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  return ~(pat0 | pat1);
}

// @brief 内容をダンプする．
void
SnNor2::dump(ostream& s) const
{
  s << "NOR2(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnNor3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor3::SnNor3(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnOr3(id, inputs)
{
}

// @brief デストラクタ
SnNor3::~SnNor3()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor3::gate_type() const
{
  return kGateNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnNor3::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  PackedVal pat2 = mFanins[2]->hval();
  return ~(pat0 | pat1 | pat2);
}

// @brief 正常値の計算を行う．
PackedVal
SnNor3::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  PackedVal pat2 = mFanins[2]->gval();
  return ~(pat0 | pat1 | pat2);
}

// @brief 故障値の計算を行う．
PackedVal
SnNor3::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  PackedVal pat2 = mFanins[2]->fval();
  return ~(pat0 | pat1 | pat2);
}

// @brief 内容をダンプする．
void
SnNor3::dump(ostream& s) const
{
  s << "NOR3(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ", " << mFanins[2]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnNor4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor4::SnNor4(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnOr4(id, inputs)
{
}

// @brief デストラクタ
SnNor4::~SnNor4()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor4::gate_type() const
{
  return kGateNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnNor4::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  PackedVal pat2 = mFanins[2]->hval();
  PackedVal pat3 = mFanins[3]->hval();
  return ~(pat0 | pat1 | pat2 | pat3);
}

// @brief 正常値の計算を行う．
PackedVal
SnNor4::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  PackedVal pat2 = mFanins[2]->gval();
  PackedVal pat3 = mFanins[3]->gval();
  return ~(pat0 | pat1 | pat2 | pat3);
}

// @brief 故障値の計算を行う．
PackedVal
SnNor4::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  PackedVal pat2 = mFanins[2]->fval();
  PackedVal pat3 = mFanins[3]->fval();
  return ~(pat0 | pat1 | pat2 | pat3);
}

// @brief 内容をダンプする．
void
SnNor4::dump(ostream& s) const
{
  s << "NOR4(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ", " << mFanins[2]->id();
  s << ", " << mFanins[3]->id();
  s << ")" << endl;
}

END_NAMESPACE_YM_SATPG_FSIM2
