
/// @file dtpg_cmd.cc
/// @brief dtpg_cmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "LuaDruid.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// テストパタン生成を行う．
int
dtpg_cmd(
  lua_State* L
)
{
  LuaDruid lua{L};

}

END_NONAMESPACE


void
LuaDruid::init_dtpg()
{
  // コマンドを登録する．
  reg_cfunction("druid", "dtpg", dtpg_cmd);
}

END_NAMESPACE_DRUID
