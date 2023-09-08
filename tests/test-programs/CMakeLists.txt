
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
#  TpgNetworkTest
# ===================================================================

set ( TpgNetworkTest_SOURCES
  tpgnetworktest/TpgNetworkTest.cc
  )

add_executable ( TpgNetworkTest
  ${TpgNetworkTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( TpgNetworkTest
  PRIVATE ${PROJECT_SOURCE_DIR}/c++-src/tpg_network
  )

target_compile_options ( TpgNetworkTest
  PRIVATE "-O3"
  )

target_compile_definitions ( TpgNetworkTest
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( TpgNetworkTest
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TpgNetworkTest
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( TpgNetworkTest_d
  ${TpgNetworkTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_include_directories ( TpgNetworkTest_d
  PRIVATE ${PROJECT_SOURCE_DIR}/c++-src/tpg_network
  )

target_compile_options ( TpgNetworkTest_d
  PRIVATE "-g"
  )

target_link_libraries ( TpgNetworkTest_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TpgNetworkTest_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  TpgFaultMgrTest
# ===================================================================

set ( TpgFaultMgrTest_SOURCES
  tpgfaultmgrtest/TpgFaultMgrTest.cc
  )

add_executable ( TpgFaultMgrTest
  ${TpgFaultMgrTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( TpgFaultMgrTest
  PRIVATE ${PROJECT_SOURCE_DIR}/c++-src/tpg_network
  )

target_compile_options ( TpgFaultMgrTest
  PRIVATE "-O3"
  )

target_compile_definitions ( TpgFaultMgrTest
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( TpgFaultMgrTest
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TpgFaultMgrTest
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( TpgFaultMgrTest_d
  ${TpgFaultMgrTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_include_directories ( TpgFaultMgrTest_d
  PRIVATE ${PROJECT_SOURCE_DIR}/c++-src/tpg_network
  )

target_compile_options ( TpgFaultMgrTest_d
  PRIVATE "-g"
  )

target_link_libraries ( TpgFaultMgrTest_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TpgFaultMgrTest_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  FsimTest
# ===================================================================

set ( FsimTest_SOURCES
  fsimtest/FsimTest.cc
  )

add_executable ( FsimTest
  ${FsimTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_fsim_main_obj>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj>
  $<TARGET_OBJECTS:druid_fsimbside2_obj>
  $<TARGET_OBJECTS:druid_fsimbside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( FsimTest
  PRIVATE "-O3"
  )

target_compile_definitions ( FsimTest
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( FsimTest
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( FsimTest
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( FsimTest_d
  ${FsimTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_fsim_main_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( FsimTest_d
  PRIVATE "-g"
  )

target_link_libraries ( FsimTest_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( FsimTest_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  dtpg_test
# ===================================================================

add_executable ( dtpg_test
  dtpgtest/dtpg_test.cc
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_main_obj>
  $<TARGET_OBJECTS:druid_fsim_main_obj>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj>
  $<TARGET_OBJECTS:druid_fsimbside2_obj>
  $<TARGET_OBJECTS:druid_fsimbside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( dtpg_test
  PRIVATE "-O3"
  )

target_compile_definitions ( dtpg_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( dtpg_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dtpg_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( dtpg_test_d
  dtpgtest/dtpg_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_fsim_main_obj_d>
  $<TARGET_OBJECTS:druid_main_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( dtpg_test_d
  PRIVATE "-g"
  )

target_link_libraries ( dtpg_test_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dtpg_test_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


if ( 0 )
# ===================================================================
#  mf_comp
# ===================================================================

set ( mf_comp_SOURCES
  mf_comp/mf_comp.cc
  )

add_executable ( mf_comp
  ${mf_comp_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj>
  $<TARGET_OBJECTS:druid_fsimbside2_obj>
  $<TARGET_OBJECTS:druid_fsimbside3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( mf_comp
  PRIVATE "-O3"
  )

target_compile_definitions ( mf_comp
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( mf_comp
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( mf_comp
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( mf_comp_d
  ${mf_comp_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  $<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  $<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( mf_comp_d
  PRIVATE "-g"
  )

target_link_libraries ( mf_comp_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( mf_comp_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

endif ( 0 )

# ===================================================================
#  read_blif_test
# ===================================================================

add_executable ( tpg_read_blif_test
  read_blif/read_blif_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( tpg_read_blif_test
  PRIVATE "-g"
  )

target_link_libraries ( tpg_read_blif_test
  ${YM_LIB_DEPENDS}
  )

# ===================================================================
#  read_iscas89_test
# ===================================================================

add_executable ( tpg_read_iscas89_test
  read_iscas89/read_iscas89_test.cc
  $<TARGET_OBJECTS:druid_types_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimcombi3_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside2_obj_d>
  #$<TARGET_OBJECTS:druid_fsimbside3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( tpg_read_iscas89_test
  PRIVATE "-g"
  )

target_link_libraries ( tpg_read_iscas89_test
  ${YM_LIB_DEPENDS}
  )
