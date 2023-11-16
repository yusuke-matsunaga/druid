
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

ym_add_gtest ( druid_SimNodeSA2Test
  SimNodeVal2Test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( druid_SimNodeSA2Test
  PRIVATE "-DFSIM_VAL2" "-DFSIM_COMBI"
  )

ym_add_gtest ( druid_SimNodeSA3Test
  SimNodeVal3Test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( druid_SimNodeSA3Test
  PRIVATE "-DFSIM_VAL3" "-DFSIM_COMBI"
  )

ym_add_gtest ( druid_SimNodeTD2Test
  SimNodeVal2Test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( druid_SimNodeTD2Test
  PRIVATE "-DFSIM_VAL2" "-DFSIM_BSIDE"
  )

ym_add_gtest ( druid_SimNodeTD3Test
  SimNodeVal3Test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_definitions ( druid_SimNodeTD3Test
  PRIVATE "-DFSIM_VAL3" "-DFSIM_BSIDE"
  )
