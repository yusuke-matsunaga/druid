
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
#  classify_test
# ===================================================================

add_executable ( classify_test
  main.cc
  Classifier2.cc
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

target_compile_options ( classify_test
  PRIVATE "-O3"
  )

target_compile_definitions ( classify_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( classify_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( classify_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( classify_test_d
  main.cc
  Classifier2.cc
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

target_compile_options ( classify_test_d
  PRIVATE "-g"
  )

target_link_libraries ( classify_test_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( classify_test_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()
