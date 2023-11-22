
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
#  TpgFaultMgrTest
# ===================================================================

set ( TpgFaultMgrTest_SOURCES
  TpgFaultMgrTest.cc
  )

add_executable ( TpgFaultMgrTest
  ${TpgFaultMgrTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
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
