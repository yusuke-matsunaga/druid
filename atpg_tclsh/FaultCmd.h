﻿#ifndef FAULTCMD_H
#define FAULTCMD_H

/// @file FaultCmd.h
/// @brief FaultCmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"


BEGIN_NAMESPACE_SATPG

//////////////////////////////////////////////////////////////////////
// 故障の設定を行うコマンド
//////////////////////////////////////////////////////////////////////
class SetFault :
  public AtpgCmd
{
public:

  /// @brief コンストラクタ
  SetFault(AtpgMgr* mgr);


protected:

  // コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);

};

END_NAMESPACE_SATPG

#endif // FAULTCMD_H
