
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

target_compile_definitions ( SaFsim2SimNodeTest
  PRIVATE "-DFSIM_VAL2" "-DFSIM_SA"
  )
