
/// @file LuaDruid_TpgNetwork.cc
/// @brief LuaDruid_TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "LuaDruid.h"
#include "TpgNetwork.h"
#include "ym/ClibCellLibrary.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// TpgNetwork 用のシグネチャ
const char* TPG_SIGNATURE{"Druid.TpgNetwork"};

// TpgNetwork を生成する共通関数
TpgNetwork*
tpg_new(
  Luapp& lua
)
{
  // メモリ領域は Lua で確保する．
  void* p = lua.new_userdata(sizeof(TpgNetwork));
  auto tpgnet = new (p) TpgNetwork{};

  // TpgNetwork 用の metatable を取ってくる．
  lua.L_getmetatable(TPG_SIGNATURE);

  // それを今作った userdata にセットする．
  lua.set_metatable(-2);

  return tpgnet;
}

// TpgNetwork 用のデストラクタ
int
tpg_gc(
  lua_State* L
)
{
  LuaDruid lua{L};

  auto tpgnet = lua.to_tpgnetwork(1);
  if ( tpgnet ) {
    // デストラクタを明示的に起動する．
    tpgnet->~TpgNetwork();
  }

  // メモリは Lua が開放する．
  return 0;
}

// blif ファイルを読み込む．
int
tpg_read_blif(
  lua_State* L
)
{
  LuaDruid lua{L};

  string clock_str{};
  string reset_str{};
  ClibCellLibrary library{};
  string filename{};

  int n = lua.get_top();
  if ( n == 1 ) {
    // ファイル名のみ
    if ( !lua.is_string(1) ) {
      return lua.error_end("Error in read_blif(): 1st argument should be a string.");
    }
    filename = lua.to_string(1);
  }
  else if ( n == 2 ) {
    // ファイル名とパラメータのテーブル
    if ( !lua.is_string(1) ) {
      return lua.error_end("Error in read_blif(): 1st argument should be a string.");
    }
    filename = lua.to_string(1);

    const int TABLE_INDEX = 2;
    if ( !lua.is_table(TABLE_INDEX) ) {
      return lua.error_end("Error in read_blif(): 2nd argument should be a table.");
    }
    auto ret = lua.get_field(TABLE_INDEX, "cell_library");
    if ( ret == LUA_TNIL ) {
      // 存在しなかった．
      // エラーではない．
      ;
    }
    else {
      auto lib = lua.to_clib(-1);
      if ( lib == nullptr ) {
	return lua.error_end("Error in read_blif(): ClibCellLibrary required for 'cell_library' field.");
      }
      library = *lib;
    }
    // get_filed(TABLE_INDEX, "cell_library") で積まれた要素を棄てる．
    lua.pop(1);

    if ( lua.get_string_field(TABLE_INDEX, "clock", clock_str) == Luapp::ERROR ) {
      return lua.error_end("Error in read_blif(): Illegal value for 'clock' field in 2nd argument.");
    }
    if ( lua.get_string_field(TABLE_INDEX, "reset", reset_str) == Luapp::ERROR ) {
      return lua.error_end("Error in read_blif(): Illegal value for 'reset' field in 2nd argument.");
    }
  }
  else {
    return lua.error_end("Error: read_blif() expects one or two arguments.");
  }

  try {
    auto tpgnet = tpg_new(lua);
    if ( !tpgnet->read_blif(filename, library) ) {
      // エラーが起こった．
      return lua.error_end("Error: read_blif()");
    }

    return 1;
  }
  catch ( BnetError& error ) {
    return lua.error_end("Error: read_blif() failed.");
  }
}

// iscas89 ファイルを読み込む．
int
tpg_read_iscas89(
  lua_State* L
)
{
  LuaDruid lua{L};

  string filename{};
  string clock_str{};

  int n = lua.get_top();
  if ( n == 1 ) {
    // ファイル名を引数にとる．
    if ( !lua.is_string(1) ) {
      return lua.error_end("Error in read_iscas89(): 1st argument should be a string.");
    }
    filename = lua.to_string(1);
  }
  else if ( n == 2 ) {
    // ファイル名とパラメータのテーブル
    if ( !lua.is_string(1) ) {
      return lua.error_end("Error in read_iscas89(): 1st argument should be a string.");
    }
    filename = lua.to_string(1);

    const int TABLE_INDEX = 2;
    if ( !lua.is_table(TABLE_INDEX) ) {
      return lua.error_end("Error in read_iscas89(): 2nd argument should be a table.");
    }

    if ( lua.get_string_field(TABLE_INDEX, "clock", clock_str) == Luapp::ERROR ) {
      return lua.error_end("Error in read_iscas89(): Illegal value for 'clock' field in 2nd argument.");
    }

  }
  else {
    return lua.error_end("Error: read_iscas89() expects one or two arguments.");
  }

  try {
    auto tpgnet = tpg_new(lua);
    if ( !tpgnet->read_iscas89(filename) ) {
      // エラーが起こった．
      return lua.error_end("Error: read_iscas89()");
    }

    return 1;
  }
  catch ( BnetError& error ) {
    return lua.error_end("Error: read_iscas89() failed.");
  }
}

END_NONAMESPACE

// @brief 対象が TpgNetwork の時 true を返す．
bool
LuaDruid::is_tpgnetwork(
  int idx
)
{
  auto tpgnet = to_tpgnetwork(idx);
  return tpgnet != nullptr;
}

// @brief 対象を TpgNetwork として取り出す．
TpgNetwork*
LuaDruid::to_tpgnetwork(
  int idx
)
{
  void* p = L_checkudata(idx, TPG_SIGNATURE);
  return reinterpret_cast<TpgNetwork*>(p);
}

// @brief TpgNetwork 関係の初期化を行う．
void
LuaDruid::init_TpgNetwork(
  vector<struct luaL_Reg>& mylib
)
{
  // メンバ関数(メソッド)テーブル
  static const struct luaL_Reg mt[] = {
    {nullptr, nullptr}
  };

  // TpgNetwork に対応する Lua の metatable を作る．
  L_newmetatable(TPG_SIGNATURE);

  // metatable 自身を __index に登録する．
  push_value(-1);
  set_field(-2, "__index");

  // デストラクタを __gc に登録する．
  push_cfunction(tpg_gc);
  set_field(-2, "__gc");

  // メソッドテーブルを登録する．
  L_setfuncs(mt, 0);

  // 生成関数を登録する．
  mylib.push_back({"read_blif", tpg_read_blif});
  mylib.push_back({"read_iscas89", tpg_read_iscas89});
}

END_NAMESPACE_DRUID
