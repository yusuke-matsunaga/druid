
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
#  fsim_test
# ===================================================================

set ( fsim_test_SOURCES
  fsim_test.cc
  )

add_executable ( fsim_test
  ${fsim_test_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj>
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

target_compile_options ( fsim_test
  PRIVATE "-O3"
  )

target_compile_definitions ( fsim_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( fsim_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( fsim_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( fsim_test_d
  ${fsim_test_SOURCES}
  $<TARGET_OBJECTS:druid_types_obj_d>
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

target_compile_options ( fsim_test_d
  PRIVATE "-g" "-fsanitize=address"
  )

target_link_libraries ( fsim_test_d
  ${YM_LIB_DEPENDS}
  )

target_link_options ( fsim_test_d
  PRIVATE "-fsanitize=address"
  )
