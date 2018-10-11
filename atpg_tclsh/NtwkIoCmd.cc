﻿
/// @file NtwkIoCmd.cc
/// @brief TgNetwork 操作関連のコマンド
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "NtwkIoCmd.h"
#include "TpgNetwork.h"
#include "AtpgMsgHandler.h"
#include "AtpgMgr.h"

#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// blif 形式のファイルを読み込むコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ReadBlif::ReadBlif(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  set_usage_string("filename");
}

// @brief デストラクタ
ReadBlif::~ReadBlif()
{
}

// コマンド処理関数
int
ReadBlif::cmd_proc(TclObjVector& objv)
{
  int objc = objv.size();

  // このコマンドはファイル名を引数としてとる．
  if ( objc != 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  string filename = objv[1];
  // ファイル名の展開を行う．
  string ex_filename;
  bool stat1 = tilde_subst(filename, ex_filename);
  if ( !stat1 ) {
    // ファイル名の文字列に誤りがあった．
    return TCL_ERROR;
  }

  AtpgMsgHandler mh;
  MsgMgr::attach_handler(&mh);

  bool stat = _network().read_blif(ex_filename);

  MsgMgr::detach_handler(&mh);

  if ( stat ) {
    after_set_network();
  }
  else {
    set_result(mh.msg_obj());
    return TCL_ERROR;
  }

  return TCL_OK;
}


//////////////////////////////////////////////////////////////////////
// ISCAS89 形式のファイルを読み込むコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ReadIscas89::ReadIscas89(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  set_usage_string("filename");
}

// @brief デストラクタ
ReadIscas89::~ReadIscas89()
{
}

// コマンド処理関数
int
ReadIscas89::cmd_proc(TclObjVector& objv)
{
  int objc = objv.size();

  // このコマンドはファイル名を引数としてとる．
  if ( objc != 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  string filename = objv[1];
  // ファイル名の展開を行う．
  string ex_filename;
  bool stat1 = tilde_subst(filename, ex_filename);
  if ( !stat1 ) {
    // ファイル名の文字列に誤りがあった．
    return TCL_ERROR;
  }

  AtpgMsgHandler mh;
  MsgMgr::attach_handler(&mh);

  bool stat2 = _network().read_iscas89(ex_filename);

  MsgMgr::detach_handler(&mh);

  if ( stat2 ) {
    after_set_network();
  }
  else {
    set_result(mh.msg_obj());
    return TCL_ERROR;
  }

  return TCL_OK;
}


//////////////////////////////////////////////////////////////////////
// TgNetwork の内容を出力するコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
WriteNetwork::WriteNetwork(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  set_usage_string("?filename?");
}

// コマンド処理関数
int
WriteNetwork::cmd_proc(TclObjVector& objv)
{
  int objc = objv.size();

  // このコマンドはファイル名のみを引数に取る．
  // 引数がなければ標準出力に出す．
  if ( objc > 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  // 出力先のファイルを開く
  ostream* osp = &cout;
  ofstream ofs;
  if ( objc == 2 ) {
    string filename = objv[1];
    if ( !open_ofile(ofs, filename) ) {
      // ファイルが開けなかった．
      return TCL_ERROR;
    }
    osp = &ofs;
  }

  print_network(*osp, _network());

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
