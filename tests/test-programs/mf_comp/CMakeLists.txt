
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
#  mf_comp
# ===================================================================

set ( mf_comp_SOURCES
  mf_comp.cc
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
