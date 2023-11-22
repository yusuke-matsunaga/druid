
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
  TpgNetworkTest.cc
  )

add_executable ( TpgNetworkTest
  ${TpgNetworkTest_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
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
