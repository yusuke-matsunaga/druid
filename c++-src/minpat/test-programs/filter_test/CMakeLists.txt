
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

add_executable ( filter_test
  filter_test.cc
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_minpat_obj>
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

target_compile_options ( filter_test
  PRIVATE "-O3"
  )

target_compile_definitions ( filter_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( filter_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( filter_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

add_executable ( filter_test_d
  filter_test.cc
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
  )

target_compile_options ( filter_test_d
  PRIVATE "-g" "-fsanitize=address"
  )

target_link_libraries ( filter_test_d
  ${YM_LIB_DEPENDS}
  )

target_link_options ( filter_test_d
  PRIVATE "-fsanitize=address"
  )

add_executable ( filter_test2
  filter_test2.cc
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_minpat_obj>
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

target_compile_options ( filter_test2
  PRIVATE "-O3"
  )

target_compile_definitions ( filter_test2
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( filter_test2
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( filter_test2
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

add_executable ( filter_test2_d
  filter_test2.cc
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
  )

target_compile_options ( filter_test2_d
  PRIVATE "-g"
  )

target_link_libraries ( filter_test2_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( filter_test2_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()
