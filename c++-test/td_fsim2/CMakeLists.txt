
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

add_executable ( TdFsim2SimNodeTest
  SimNodeTest.cc
  $<TARGET_OBJECTS:druid_common_ad>
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_combopt_ad>
  )

target_compile_options ( TdFsim2SimNodeTest
  PRIVATE "-g"
  )

target_link_libraries ( TdFsim2SimNodeTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( TdFsim2Test
  TdFsim2SimNodeTest
  )
