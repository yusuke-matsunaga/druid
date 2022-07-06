
/// @file druid_lua.cc
/// @brief druid_lua の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "LuaDruid.h"

int
main(
  int argc,
  char** argv
)
{
  using namespace std;
  using DRUID_NAMESPACE::LuaDruid;

  LuaDruid lua;

  lua.L_openlibs();
  lua.open_Druid();

  return lua.main_loop(argc, argv);
}
