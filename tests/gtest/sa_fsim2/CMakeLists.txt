
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

ym_add_gtest ( SaFsim2SimNodeTest
  SimNodeTest.cc
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( SaFsim2SimNodeTest
  PRIVATE "-DFSIM_VAL2" "-DFSIM_SA"
  )
