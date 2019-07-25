
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/c++-src/tpg_network
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

ym_add_gtest ( TpgNodeTest
  TpgNodeTest.cc
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )

ym_add_gtest(GateTypeTest
  GateTypeTest.cc
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )

ym_add_gtest(GateEncTest
  GateEncTest.cc
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )

ym_add_gtest(FaultyGateEncTest
  FaultyGateEncTest.cc
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )
