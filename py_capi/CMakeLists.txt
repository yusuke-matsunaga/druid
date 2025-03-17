
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/fsim
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/bist
  ${PROJECT_SOURCE_DIR}/include/classify
  ${PROJECT_SOURCE_DIR}/include/condgen
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BUILD_DIR}/include
  ${Python3_INCLUDE_DIRS}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( types )
add_subdirectory ( fsim )
add_subdirectory ( main )
add_subdirectory ( dtpg )
add_subdirectory ( bist )
add_subdirectory ( condgen )
add_subdirectory ( tests )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( druid_SOURCES
  druid.cc
  druid_module.cc
  ${types_SOURCES}
  ${fsim_SOURCES}
  ${main_SOURCES}
  ${dtpg_SOURCES}
  ${bist_SOURCES}
  ${condgen_SOURCES}
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

add_executable ( druid
  ${druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj>
  $<TARGET_OBJECTS:py_ymcell_obj>
  $<TARGET_OBJECTS:py_ymsat_obj>
  $<TARGET_OBJECTS:py_ymlogic_obj>
  $<TARGET_OBJECTS:druid_types_obj>
  $<TARGET_OBJECTS:druid_dtpg_obj>
  $<TARGET_OBJECTS:druid_classify_obj>
  $<TARGET_OBJECTS:druid_condgen_obj>
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

target_include_directories ( druid
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( druid
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_executable ( druid_d
  ${druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj_d>
  $<TARGET_OBJECTS:py_ymcell_obj_d>
  $<TARGET_OBJECTS:py_ymsat_obj_d>
  $<TARGET_OBJECTS:py_ymlogic_obj_d>
  $<TARGET_OBJECTS:druid_types_obj_d>
  $<TARGET_OBJECTS:druid_dtpg_obj_d>
  $<TARGET_OBJECTS:druid_classify_obj_d>
  $<TARGET_OBJECTS:druid_condgen_obj_d>
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

target_compile_options ( druid_d
  PRIVATE "-g"
  )

target_include_directories ( druid_d
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( druid_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install ( TARGETS druid DESTINATION bin )
install ( TARGETS druid_d DESTINATION bin )
