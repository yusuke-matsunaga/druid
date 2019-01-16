

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${YM_SUBMODULE_INCLUDE_LIST}
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( debug )


# ===================================================================
#  cython 用のソースファイルの設定
# ===================================================================

set ( CYTHON_SOURCES
  ${PROJECT_SOURCE_DIR}/cython-src/druid_core.pyx
  )

set_source_files_properties (
  ${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE
  )


# ===================================================================
#  module ターゲットの設定
# ===================================================================

cython_add_module ( druid_core
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:druid_common>
  $<TARGET_OBJECTS:druid_fsimsa2>
  $<TARGET_OBJECTS:druid_fsimsa3>
  $<TARGET_OBJECTS:druid_fsimtd2>
  $<TARGET_OBJECTS:druid_fsimtd3>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( druid_core
  PRIVATE "-O3"
  )

target_compile_definitions ( druid_core
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( druid_core
  ${YM_LIB_DEPENDS}
  )


cython_add_module ( druid_core_d
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:druid_common_d>
  $<TARGET_OBJECTS:druid_fsimsa2_d>
  $<TARGET_OBJECTS:druid_fsimsa3_d>
  $<TARGET_OBJECTS:druid_fsimtd2_d>
  $<TARGET_OBJECTS:druid_fsimtd3_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( druid_core_d
  PRIVATE "-g"
  )

target_link_libraries ( druid_core_d
  ${YM_LIB_DEPENDS}
  )

# debug/druid_core.so を druid_core_d.so の別名にする．
# するとライブラリパスの変更で
# import druid_core のままリリース版/デバッグ版の切り替えが行える．
add_custom_command (
  TARGET druid_core_d
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy $<TARGET_FILE:druid_core_d> ${CMAKE_CURRENT_BINARY_DIR}/debug/druid_core.so
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install ( TARGETS druid_core
  DESTINATION "${YM_PYTHON_LIBDIR}"
  )

install ( FILES
  __init__.py
  compaction.py
  dtpg.py
  minpat.py
  DESTINATION "${YM_PYTHON_LIBDIR}"
  )