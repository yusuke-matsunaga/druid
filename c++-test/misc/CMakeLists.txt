
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
# テストターゲットの定義
# ===================================================================

ym_add_gtest(Val3Test
  Val3Test.cc
  $<TARGET_OBJECTS:druid_common_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_ad>
  ${YM_SUBMODULE_OBJ_AD_LIST}
  )
