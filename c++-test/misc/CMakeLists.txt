
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
  $<TARGET_OBJECTS:druid_common_ad>
  $<TARGET_OBJECTS:druid_fsimsa2_ad>
  $<TARGET_OBJECTS:druid_fsimsa3_ad>
  $<TARGET_OBJECTS:druid_fsimtd2_ad>
  $<TARGET_OBJECTS:druid_fsimtd3_ad>
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_combopt_ad>
  )
