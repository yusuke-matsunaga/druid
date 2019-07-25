
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
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )

target_compile_definitions ( SaFsim3SimNodeTest
  PRIVATE "-DFSIM_VAL3" "FSIM_SA"
  )
