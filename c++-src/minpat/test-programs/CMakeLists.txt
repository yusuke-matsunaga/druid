
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ../
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================




# ===================================================================
#  ソースファイルの設定
# ===================================================================

add_executable ( minpat
  minpat.cc
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

target_compile_options ( minpat
  PRIVATE "-O3"
  )

target_compile_definitions ( minpat
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( minpat
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( minpat
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

add_executable ( minpat_d
  minpat.cc
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

target_compile_options ( minpat_d
  PRIVATE "-g"
  )

target_link_libraries ( minpat_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( minpat_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

add_executable ( dichotomy_test
  dichotomy_test.cc
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

target_compile_options ( dichotomy_test
  PRIVATE "-O3"
  )

target_compile_definitions ( dichotomy_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( dichotomy_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dichotomy_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()

add_executable ( dichotomy_test_d
  dichotomy_test.cc
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

target_compile_options ( dichotomy_test_d
  PRIVATE "-g"
  )

target_link_libraries ( dichotomy_test_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dichotomy_test_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()
