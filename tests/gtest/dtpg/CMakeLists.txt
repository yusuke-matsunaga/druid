
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

ym_add_gtest ( druid_dtpg_test
  dtpg_test.cc
  $<TARGET_OBJECTS:tests_common_obj_d>
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/tests/data/\""
  )

target_include_directories ( druid_dtpg_test
  PRIVATE ../../common
  )

ym_add_gtest ( druid_dtpg_test2
  dtpg_test2.cc
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DDATAPATH=\"${PROJECT_SOURCE_DIR}/tests/data/\""
  )
