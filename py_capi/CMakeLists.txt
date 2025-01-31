
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/fsim
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/bist
  ${PROJECT_SOURCE_DIR}/include/classify
  ${PROJECT_SOURCE_DIR}/include/condgen
  ${PROJECT_SOURCE_DIR}/include/pym
  ${PROJECT_BUILD_DIR}/include
  ${Python3_INCLUDE_DIRS}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( tests )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( druid_SOURCES
  druid.cc
  druid_module.cc

  types/types_module.cc
  types/PyVal3.cc
  types/PyFaultType.cc
  types/PyFaultStatus.cc
  types/PyBitVector.cc
  types/PyTestVector.cc
  types/PyInputVector.cc
  types/PyDffVector.cc
  types/PyTpgFault.cc
  types/PyTpgNetwork.cc
  types/PyTpgNode.cc
  types/PyTpgMFFC.cc
  types/PyTpgFFR.cc
  types/PyAssignExpr.cc

  fsim/fsim_module.cc
  fsim/PyFsim.cc
  fsim/PyDiffBits.cc
  fsim/PyDiffBitsArray.cc

  main/main_module.cc
  main/PyClassifier.cc

  dtpg/dtpg_module.cc
  dtpg/PyDtpgMgr.cc
  dtpg/PyDtpgResult.cc
  dtpg/PyDtpgStats.cc

  bist/bist_module.cc
  bist/PyPhaseShifter.cc
  bist/PyLFSR.cc

  #condgen/condgen_module.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

add_executable ( druid
  ${druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj>
  $<TARGET_OBJECTS:py_ymcell_obj>
  $<TARGET_OBJECTS:py_ymsat_obj>
  $<TARGET_OBJECTS:py_ymlogic_obj>
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_classify_obj>
  $<TARGET_OBJECTS:druid_condgen_obj>
  $<TARGET_OBJECTS:druid_fsim_obj>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( druid
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( druid
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_executable ( druid_d
  ${druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj_d>
  $<TARGET_OBJECTS:py_ymcell_obj_d>
  $<TARGET_OBJECTS:py_ymsat_obj_d>
  $<TARGET_OBJECTS:py_ymlogic_obj_d>
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_classify_obj_d>
  $<TARGET_OBJECTS:druid_condgen_obj_d>
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
  )

target_compile_options ( druid_d
  PRIVATE "-g"
  )

target_include_directories ( druid_d
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( druid_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install ( TARGETS druid DESTINATION bin )
install ( TARGETS druid_d DESTINATION bin )
