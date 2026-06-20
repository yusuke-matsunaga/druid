
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE}
  ..
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

ym_add_gtest ( druid_FaultAnalyzeTest
  FaultAnalyzeTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )


ym_add_gtest ( druid_FaultAnalyzeTest2
  FaultAnalyzeTest2.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )


ym_add_gtest ( druid_DomCheckerTest
  DomCheckerTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )


ym_add_gtest ( druid_DomCheckerTest2
  DomCheckerTest2.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )


ym_add_gtest ( druid_FFRAnalyzeTest
  FFRAnalyzeTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )

if ( 0 )
ym_add_gtest ( druid_MFFCAnalyzeTest
  MFFCAnalyzeTest.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_minpat_obj_d>
  $<TARGET_OBJECTS:druid_fsim_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_d>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  DEFINITIONS "-DTESTDATA_DIR=\"${TESTDATA_DIR}\""
  )
endif ()