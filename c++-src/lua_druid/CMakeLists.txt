
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${YM_SUBMODULE_INCLUDE_LIST}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (lua_druid_SOURCES
  lua_druid.cc
  LuaDruid_TpgNetwork.cc
  dtpg_cmd.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

add_executable ( lua_druid
  ${lua_druid_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
  $<TARGET_OBJECTS:ym_luaclib_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( lua_druid
  PRIVATE
  ${LUA_INCLUDE_DIR}
  )

target_link_libraries ( lua_druid
  ${YM_LIB_DEPENDS}
  )

add_executable ( lua_druid_d
  ${lua_druid_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  $<TARGET_OBJECTS:ym_luaclib_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( lua_druid_d
  PRIVATE "-g"
  )

target_include_directories ( lua_druid_d
  PRIVATE
  ${LUA_INCLUDE_DIR}
  )

target_link_libraries ( lua_druid_d
  ${YM_LIB_DEPENDS}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
