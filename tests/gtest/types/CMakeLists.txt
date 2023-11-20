
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

ym_add_gtest ( druid_TpgNodeTest
  TpgNodeTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_include_directories ( druid_TpgNodeTest
  PRIVATE ${PROJECT_SOURCE_DIR}/c++-src/types/tpg_network
  )

ym_add_gtest( druid_read_blif_test
  read_blif_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )

ym_add_gtest( druid_read_iscas89_test
  read_iscas89_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )

ym_add_gtest( druid_xor_test
  xor_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

ym_add_gtest ( druid_TpgFaultMgrTest
  TpgFaultMgrTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )

ym_add_gtest(druid_Val3Test
  Val3Test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

ym_add_gtest ( druid_BitVectorTest
  BitVectorTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )
