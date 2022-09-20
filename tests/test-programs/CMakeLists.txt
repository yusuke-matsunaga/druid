
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
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
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
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
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
#  FsimTest
# ===================================================================

set ( FsimTest_SOURCES
  fsimtest/FsimTest.cc
  )

add_executable ( FsimTest
  ${FsimTest_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
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
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
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

set ( dtpg_test_SOURCES
  dtpgtest/dtpg_test.cc
  )

add_executable ( dtpg_test
  ${dtpg_test_SOURCES}
  $<TARGET_OBJECTS:tests_common_obj>
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( dtpg_test
  PRIVATE ../common
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
  ${dtpg_test_SOURCES}
  $<TARGET_OBJECTS:tests_common_obj_d>
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_include_directories ( dtpg_test_d
  PRIVATE ../common
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


# ===================================================================
#  mf_comp
# ===================================================================

set ( mf_comp_SOURCES
  mf_comp/mf_comp.cc
  )

add_executable ( mf_comp
  ${mf_comp_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
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
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
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
