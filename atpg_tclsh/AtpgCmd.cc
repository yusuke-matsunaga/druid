﻿
/// @file AtpgCmd.cc
/// @brief AtpgCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"
#include "AtpgMgr.h"
#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "FaultMgr.h"
#include "ym/BnNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// AtpgCmd
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AtpgCmd::AtpgCmd(AtpgMgr* mgr) :
  mMgr(mgr)
{
}

// @brief デストラクタ
AtpgCmd::~AtpgCmd()
{
}

// @brief ネットワークをセットした後に呼ぶ関数
void
AtpgCmd::after_set_network()
{
  mMgr->after_set_network();

  // 諸元を TCL 変数にセットしておく
  const TpgNetwork& network = _network();
  ymuint nn = network.node_num();
  ymuint n_buff = 0;
  ymuint n_not = 0;
  ymuint n_and = 0;
  ymuint n_and2 = 0;
  ymuint n_and3 = 0;
  ymuint n_and4 = 0;
  ymuint n_nand = 0;
  ymuint n_nand2 = 0;
  ymuint n_nand3 = 0;
  ymuint n_nand4 = 0;
  ymuint n_or = 0;
  ymuint n_or2 = 0;
  ymuint n_or3 = 0;
  ymuint n_or4 = 0;
  ymuint n_nor = 0;
  ymuint n_nor2 = 0;
  ymuint n_nor3 = 0;
  ymuint n_nor4 = 0;
  ymuint n_xor = 0;
  ymuint n_xor2 = 0;
  ymuint n_xnor = 0;
  ymuint n_xnor2 = 0;
  ymuint n_cplx = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* node = network.node(i);
    if ( !node->is_logic() ) {
      continue;
    }
    switch ( node->gate_type() ) {
    case TpgNode::kGateBUFF:
      ++ n_buff;
      break;

    case TpgNode::kGateNOT:
      ++ n_not;
      break;

    case TpgNode::kGateAND:
      ++ n_and;
      switch ( node->fanin_num() ) {
      case 2: ++ n_and2; break;
      case 3: ++ n_and3; break;
      case 4: ++ n_and4; break;
      }
      break;

    case TpgNode::kGateNAND:
      ++ n_nand;
      switch ( node->fanin_num() ) {
      case 2: ++ n_nand2; break;
      case 3: ++ n_nand3; break;
      case 4: ++ n_nand4; break;
      }
      break;

    case TpgNode::kGateOR:
      ++ n_or;
      switch ( node->fanin_num() ) {
      case 2: ++ n_or2; break;
      case 3: ++ n_or3; break;
      case 4: ++ n_or4; break;
      }
      break;

    case TpgNode::kGateNOR:
      ++ n_nor;
      switch ( node->fanin_num() ) {
      case 2: ++ n_nor2; break;
      case 3: ++ n_nor3; break;
      case 4: ++ n_nor4; break;
      }
      break;

    case TpgNode::kGateXOR:
      ++ n_xor;
      switch ( node->fanin_num() ) {
      case 2: ++ n_xor2; break;
      }
      break;

    case TpgNode::kGateXNOR:
      ++ n_xnor;
      switch ( node->fanin_num() ) {
      case 2: ++ n_xnor2; break;
      }
      break;

    default:
      ++ n_cplx;
      break;
    }
  }

  TclObj varname = "::atpg::info";
  int varflag = 0;
  set_var(varname, "input_num", network.input_num(), varflag);
  set_var(varname, "output_num", network.output_num(), varflag);
  set_var(varname, "ff_num", network.input_num2() - network.input_num(), varflag);
  set_var(varname, "logic_num", network.node_num() - network.input_num2() - network.output_num2(), varflag);
  set_var(varname, "buff_num", n_buff, varflag);
  set_var(varname, "not_num", n_not, varflag);
  set_var(varname, "and_num", n_and, varflag);
  set_var(varname, "and2_num", n_and2, varflag);
  set_var(varname, "and3_num", n_and3, varflag);
  set_var(varname, "and4_num", n_and4, varflag);
  set_var(varname, "nand_num", n_nand, varflag);
  set_var(varname, "nand2_num", n_nand2, varflag);
  set_var(varname, "nand3_num", n_nand3, varflag);
  set_var(varname, "nand4_num", n_nand4, varflag);
  set_var(varname, "or_num", n_or, varflag);
  set_var(varname, "or2_num", n_or2, varflag);
  set_var(varname, "or3_num", n_or3, varflag);
  set_var(varname, "or4_num", n_or4, varflag);
  set_var(varname, "nor_num", n_nor, varflag);
  set_var(varname, "nor2_num", n_nor2, varflag);
  set_var(varname, "nor3_num", n_nor3, varflag);
  set_var(varname, "nor4_num", n_nor4, varflag);
  set_var(varname, "xor_num", n_xor, varflag);
  set_var(varname, "xor2_num", n_xor2, varflag);
  set_var(varname, "xnor_num", n_xnor, varflag);
  set_var(varname, "xnor2_num", n_xnor2, varflag);
  set_var(varname, "cplx_num", n_cplx, varflag);
}

BEGIN_NONAMESPACE

// 故障を表す TclObj を作る
TclObj
f2obj(const TpgFault* f)
{
  TclObjVector tmp(3);
  tmp[0] = f->node()->name();
  if ( f->is_output_fault() ) {
    tmp[1] = "O";
  }
  else {
    ostringstream buf;
    buf << "I" << f->pos();
    tmp[1] = buf.str();
  }
  if ( f->val() ) {
    tmp[2] = "SA1";
  }
  else {
    tmp[2] = "SA0";
  }
  // ここでTclObjVector から TclObj へ変換を行っている．
  return tmp;
}

END_NONAMESPACE

// @brief 故障リストを更新した後に呼ぶ関数
void
AtpgCmd::after_update_faults()
{
  FaultMgr& fault_mgr = _fault_mgr();

  // 諸元を TCL 変数にセットしておく
  const vector<const TpgFault*>& remain_list = fault_mgr.remain_list();
  const vector<const TpgFault*>& untest_list = fault_mgr.untest_list();
  ymuint n_all = _network().max_fault_id();
  ymuint n_rep = _network().rep_faults().size();
  ymuint n_remain = remain_list.size();
  ymuint n_untest = untest_list.size();
  ymuint n_det = n_rep - n_remain - n_untest;

  TclObj varname = "::atpg::info";
  int varflag = 0;
  set_var(varname, "all_fault_num", n_all, varflag);
  set_var(varname, "rep_fault_num", n_rep, varflag);
  set_var(varname, "det_fault_num", n_det, varflag);
  set_var(varname, "remain_fault_num", n_remain, varflag);
  set_var(varname, "untestt_fault_num", n_untest, varflag);
#if 0
  {
    TclObjVector tmp_list;
    for (vector<const TpgFault*>::const_iterator p = all_list.begin();
	 p != all_list.end(); ++ p) {
      const TpgFault* f = *p;
      tmp_list.push_back(f2obj(f));
    }
    set_var(varname, "all_fault_list", tmp_list, varflag);
  }
  {
    TclObjVector tmp_list;
    for (vector<const TpgFault*>::const_iterator p = rep_list.begin();
	 p != rep_list.end(); ++ p) {
      const TpgFault* f = *p;
      tmp_list.push_back(f2obj(f));
    }
    set_var(varname, "rep_fault_list", tmp_list, varflag);
  }
  {
    TclObjVector tmp_list;
    for (vector<const TpgFault*>::const_iterator p = remain_list.begin();
	 p != remain_list.end(); ++ p) {
      const TpgFault* f = *p;
      tmp_list.push_back(f2obj(f));
    }
    set_var(varname, "remain_fault_list", tmp_list, varflag);
  }
  {
    TclObjVector tmp_list;
    for (vector<const TpgFault*>::const_iterator p = untest_list.begin();
	 p != untest_list.end(); ++ p) {
      const TpgFault* f = *p;
      tmp_list.push_back(f2obj(f));
    }
    set_var(varname, "untest_fault_list", tmp_list, varflag);
  }
#endif
}

// @brief TgNetwork を取り出す．
TpgNetwork&
AtpgCmd::_network()
{
  return mMgr->_network();
}

// @brief FaultMgr を取り出す．
FaultMgr&
AtpgCmd::_fault_mgr()
{
  return mMgr->_fault_mgr();
}

// @brief TvMgr を取り出す．
TvMgr&
AtpgCmd::_tv_mgr()
{
  return mMgr->_tv_mgr();
}

// @brief テストベクタのリストを取り出す．
vector<TestVector*>&
AtpgCmd::_tv_list()
{
  return mMgr->_tv_list();
}

// @brief 2値の故障シミュレータを取り出す．
Fsim&
AtpgCmd::_fsim()
{
  return mMgr->_fsim();
}

// @brief 3値の故障シミュレータを返す．
Fsim&
AtpgCmd::_fsim3()
{
  return mMgr->_fsim3();
}

// @brief ファイル読み込みに関わる時間を得る．
USTime
AtpgCmd::read_time() const
{
  return mMgr->read_time();
}

// @brief DTPG に関わる時間を得る．
USTime
AtpgCmd::dtpg_time() const
{
  return mMgr->dtpg_time();
}

// @brief 故障シミュレーションに関わる時間を得る．
USTime
AtpgCmd::fsim_time() const
{
  return mMgr->fsim_time();
}

// @brief SAT に関わる時間を得る．
USTime
AtpgCmd::sat_time() const
{
  return mMgr->sat_time();
}

// @brief その他の時間を得る．
USTime
AtpgCmd::misc_time() const
{
  return mMgr->misc_time();
}

END_NAMESPACE_YM_SATPG
