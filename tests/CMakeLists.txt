
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/enc
  ${PROJECT_SOURCE_DIR}/include/main
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_BUILD_DIR}/include
  ${YM_SUBMODULE_INCLUDE_LIST}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( gtest )
add_subdirectory ( py_druid )
add_subdirectory ( test-programs )


# ===================================================================
# テストターゲットの定義
# ===================================================================
