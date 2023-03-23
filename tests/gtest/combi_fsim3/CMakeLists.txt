
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/c++-src/fsim
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

ym_add_gtest ( SaFsim3SimNodeTest
  SimNodeTest.cc
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( SaFsim3SimNodeTest
  PRIVATE "-DFSIM_VAL3" "FSIM_COMBI"
  )
