
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
#  FsimTest
# ===================================================================

set ( FsimTest_SOURCES
  FsimTest.cc
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
