

# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${Python3_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/ymworks/py_capi/include
)


# ===================================================================
# サブディレクトリの設定
# ===================================================================
add_subdirectory( tpg_types )
add_subdirectory( fsim )
add_subdirectory( dtpg )
add_subdirectory( condgen )
add_subdirectory( pytest )


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  executable ターゲットの設定
# ===================================================================
ym_add_object_library( py_druid
  druid_module.cc
  ${tpg_types_SOURCES}
  ${fsim_SOURCES}
  ${dtpg_SOURCES}
  ${condgen_SOURCES}
  )

add_executable( druid
  druid.cc
  $<TARGET_OBJECTS:py_druid_obj>
  $<TARGET_OBJECTS:py_ymworks_obj>
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
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_condgen_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options( druid
  PRIVATE
  "-O3"
  )

target_compile_definitions( druid
  PRIVATE
  "-DNODEBUG"
  )

target_link_libraries( druid
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )
add_executable( druid_d
  druid.cc
  $<TARGET_OBJECTS:py_druid_obj_d>
  $<TARGET_OBJECTS:py_ymworks_obj_d>
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
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_condgen_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options( druid_d
  PRIVATE
  "-g"
  )

target_link_libraries( druid_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_library( druidlib
  SHARED
  druid.cc
  $<TARGET_OBJECTS:py_druid_obj_p>
  $<TARGET_OBJECTS:py_ymworks_obj_p>
  $<TARGET_OBJECTS:druid_types_obj_p>
  $<TARGET_OBJECTS:druid_fsim_obj_p>
  $<TARGET_OBJECTS:druid_fsim_multi_combi2_obj_p>
  $<TARGET_OBJECTS:druid_fsim_multi_combi3_obj_p>
  $<TARGET_OBJECTS:druid_fsim_multi_bside2_obj_p>
  $<TARGET_OBJECTS:druid_fsim_multi_bside3_obj_p>
  $<TARGET_OBJECTS:druid_fsim_naive_combi2_obj_p>
  $<TARGET_OBJECTS:druid_fsim_naive_combi3_obj_p>
  $<TARGET_OBJECTS:druid_fsim_naive_bside2_obj_p>
  $<TARGET_OBJECTS:druid_fsim_naive_bside3_obj_p>
  $<TARGET_OBJECTS:druid_dtpg_obj_p>
  $<TARGET_OBJECTS:druid_condgen_obj_p>
  ${YM_SUBMODULE_OBJ_P_LIST}
  )

target_link_libraries( druidlib
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

set_target_properties( druidlib
  PROPERTIES
  PREFIX ""
  OUTPUT_NAME "druid"
  )


# ===================================================================
# インストールターゲット
# ===================================================================

install ( TARGETS druid druid_d
  DESTINATION bin
  )

install ( TARGETS druidlib
  DESTINATION lib
  )

# ===================================================================
# テストターゲット
# ===================================================================
