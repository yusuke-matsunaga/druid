﻿
/// @file SnOr.cc
/// @brief SnOr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SnOr.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// SnOr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr::SnOr(ymuint id,
	   const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnOr::~SnOr()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr::gate_type() const
{
  return kGateOR;
}

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnOr::_calc_gval2()
{
  ymuint n = _fanin_num();
  PackedVal val = _fanin(0)->gval();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->gval();
  }
  return val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnOr::_calc_fval2()
{
  ymuint n = _fanin_num();
  PackedVal val = _fanin(0)->fval();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->fval();
  }
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
PackedVal
SnOr::_calc_gobs2(ymuint ipos)
{
  PackedVal obs = kPvAll0;
  for (ymuint i = 0; i < ipos; ++ i) {
    obs |= _fanin(i)->gval();
  }
  for (ymuint i = ipos + 1; i < _fanin_num(); ++ i) {
    obs |= _fanin(i)->gval();
  }
  return ~obs;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnOr::_calc_gval3()
{
  ymuint n = _fanin_num();
  SimNode* inode0 = _fanin(0);
  PackedVal3 val = inode0->gval3();
  for (ymuint i = 1; i < n; ++ i) {
    SimNode* inode = _fanin(i);
    val |= inode->gval3();
  }
  return val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnOr::_calc_fval3()
{
  ymuint n = _fanin_num();
  SimNode* inode0 = _fanin(0);
  PackedVal3 val = inode0->fval3();
  for (ymuint i = 1; i < n; ++ i) {
    SimNode* inode = _fanin(i);
    val |= inode->fval3();
  }
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnOr::_calc_gobs3(ymuint ipos)
{
  PackedVal obs = kPvAll1;
  for (ymuint i = 0; i < ipos; ++ i) {
    obs &= _fanin(i)->gval3().val0();
  }
  ymuint n = _fanin_num();
  for (ymuint i = ipos + 1; i < n; ++ i) {
    obs &= _fanin(i)->gval3().val0();
  }
  return obs;
}


//////////////////////////////////////////////////////////////////////
// SnOr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr2::SnOr2(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnOr2::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val = val0 | val1;
  return val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnOr2::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val = val0 | val1;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
PackedVal
SnOr2::_calc_gobs2(ymuint ipos)
{
  ymuint alt_pos = ipos ^ 1;
  return ~_fanin(alt_pos)->gval();
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnOr2::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val = val0 | val1;
  return val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnOr2::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val = val0 | val1;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnOr2::_calc_gobs3(ymuint ipos)
{
  ymuint alt_pos = ipos ^ 1;
  return _fanin(alt_pos)->gval3().val0();
}


//////////////////////////////////////////////////////////////////////
// SnOr3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr3::SnOr3(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnOr3::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val2 = inode2->gval();
  PackedVal val = val0 | val1 | val2;
  return val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnOr3::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val2 = inode2->fval();
  PackedVal val = val0 | val1 | val2;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
PackedVal
SnOr3::_calc_gobs2(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return ~(_fanin(1)->gval() | _fanin(2)->gval());
  case 1: return ~(_fanin(0)->gval() | _fanin(2)->gval());
  case 2: return ~(_fanin(0)->gval() | _fanin(1)->gval());
  }
  return kPvAll0;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnOr3::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val2 = inode2->gval3();
  PackedVal3 val = val0 | val1 | val2;
  return val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnOr3::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val2 = inode2->fval3();
  PackedVal3 val = val0 | val1 | val2;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnOr3::_calc_gobs3(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return _fanin(1)->gval3().val0() & _fanin(2)->gval3().val0();
  case 1: return _fanin(0)->gval3().val0() & _fanin(2)->gval3().val0();
  case 2: return _fanin(0)->gval3().val0() & _fanin(1)->gval3().val0();
  default: ASSERT_NOT_REACHED; break;
  }
  return kPvAll0;
}


//////////////////////////////////////////////////////////////////////
// SnOr4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr4::SnOr4(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnOr4::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val2 = inode2->gval();
  PackedVal val3 = inode3->gval();
  PackedVal val = val0 | val1 | val2 | val3;
  return val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnOr4::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val2 = inode2->fval();
  PackedVal val3 = inode3->fval();
  PackedVal val = val0 | val1 | val2 | val3;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
PackedVal
SnOr4::_calc_gobs2(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return ~(_fanin(1)->gval() | _fanin(2)->gval() | _fanin(3)->gval());
  case 1: return ~(_fanin(0)->gval() | _fanin(2)->gval() | _fanin(3)->gval());
  case 2: return ~(_fanin(0)->gval() | _fanin(1)->gval() | _fanin(3)->gval());
  case 3: return ~(_fanin(0)->gval() | _fanin(1)->gval() | _fanin(2)->gval());
  }
  return kPvAll0;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnOr4::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val2 = inode2->gval3();
  PackedVal3 val3 = inode3->gval3();
  PackedVal3 val = val0 | val1 | val2 | val3;
  return val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnOr4::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val2 = inode2->fval3();
  PackedVal3 val3 = inode3->fval3();
  PackedVal3 val = val0 | val1 | val2 | val3;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnOr4::_calc_gobs3(ymuint ipos)
{
  switch ( ipos ) {
  case 0: return _fanin(1)->gval3().val0() & _fanin(2)->gval3().val0() & _fanin(3)->gval3().val0();
  case 1: return _fanin(0)->gval3().val0() & _fanin(2)->gval3().val0() & _fanin(3)->gval3().val0();
  case 2: return _fanin(0)->gval3().val0() & _fanin(1)->gval3().val0() & _fanin(3)->gval3().val0();
  case 3: return _fanin(0)->gval3().val0() & _fanin(1)->gval3().val0() & _fanin(2)->gval3().val0();
  default: ASSERT_NOT_REACHED; break;
  }
  return kPvAll0;
}


//////////////////////////////////////////////////////////////////////
// SnNor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor::SnNor(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnNor::_calc_gval2()
{
  ymuint n = _fanin_num();
  PackedVal val = _fanin(0)->gval();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->gval();
  }
  return ~val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnNor::_calc_fval2()
{
  ymuint n = _fanin_num();
  PackedVal val = _fanin(0)->fval();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->fval();
  }
  return ~val;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnNor::_calc_gval3()
{
  ymuint n = _fanin_num();
  SimNode* inode0 = _fanin(0);
  PackedVal3 val = inode0->gval3();
  for (ymuint i = 1; i < n; ++ i) {
    SimNode* inode = _fanin(i);
    val |= inode->gval3();
  }
  return ~val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnNor::_calc_fval3()
{
  ymuint n = _fanin_num();
  SimNode* inode0 = _fanin(0);
  PackedVal3 val = inode0->fval3();
  for (ymuint i = 1; i < n; ++ i) {
    SimNode* inode = _fanin(i);
    val |= inode->fval3();
  }
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor2::SnNor2(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnNor2::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val = val0 | val1;
  return ~val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnNor2::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val = val0 | val1;
  return ~val;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnNor2::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val = val0 | val1;
  return ~val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnNor2::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val = val0 | val1;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor3::SnNor3(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnNor3::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val2 = inode2->gval();
  PackedVal val = val0 | val1 | val2;
  return ~val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnNor3::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val2 = inode2->fval();
  PackedVal val = val0 | val1 | val2;
  return ~val;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnNor3::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val2 = inode2->gval3();
  PackedVal3 val = val0 | val1 | val2;
  return ~val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnNor3::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val2 = inode2->fval3();
  PackedVal3 val = val0 | val1 | val2;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor4::SnNor4(ymuint id,
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

// @brief 正常値の計算を行う．(2値版)
PackedVal
SnNor4::_calc_gval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal val0 = inode0->gval();
  PackedVal val1 = inode1->gval();
  PackedVal val2 = inode2->gval();
  PackedVal val3 = inode3->gval();
  PackedVal val = val0 | val1 | val2 | val3;
  return ~val;
}

// @brief 故障値の計算を行う．(2値版)
PackedVal
SnNor4::_calc_fval2()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal val0 = inode0->fval();
  PackedVal val1 = inode1->fval();
  PackedVal val2 = inode2->fval();
  PackedVal val3 = inode3->fval();
  PackedVal val = val0 | val1 | val2 | val3;
  return ~val;
}

// @brief 正常値の計算を行う．(3値版)
PackedVal3
SnNor4::_calc_gval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal3 val0 = inode0->gval3();
  PackedVal3 val1 = inode1->gval3();
  PackedVal3 val2 = inode2->gval3();
  PackedVal3 val3 = inode3->gval3();
  PackedVal3 val = val0 | val1 | val2 | val3;
  return ~val;
}

// @brief 故障値の計算を行う．(3値版)
PackedVal3
SnNor4::_calc_fval3()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  PackedVal3 val0 = inode0->fval3();
  PackedVal3 val1 = inode1->fval3();
  PackedVal3 val2 = inode2->fval3();
  PackedVal3 val3 = inode3->fval3();
  PackedVal3 val = val0 | val1 | val2 | val3;
  return ~val;
}

END_NAMESPACE_YM_SATPG_FSIM
