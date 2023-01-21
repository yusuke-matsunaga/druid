
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${YM_SUBMODULE_INCLUDE_LIST}
  ${Python3_INCLUDE_DIRS}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( py_druid_SOURCES
  py_druid.cc
  druid_module.cc
  types/types_module.cc
  types/PyVal3.cc
  types/PyFaultType.cc
  types/PyFaultStatus.cc
  types/PyTestVector.cc
  types/PyInputVector.cc
  types/PyDffVector.cc
  types/PyTpgFault.cc
  types/PyTpgNetwork.cc
  fsim/fsim_module.cc
  fsim/PyFsim.cc
  dtpg/dtpg_module.cc
  dtpg/PyDtpgFFR.cc
  dtpg/PyDtpgMFFC.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

add_executable ( py_druid
  ${py_druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj>
  $<TARGET_OBJECTS:py_ymcell_obj>
  $<TARGET_OBJECTS:py_ymsat_obj>
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_include_directories ( py_druid
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( py_druid
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_executable ( py_druid_d
  ${py_druid_SOURCES}
  $<TARGET_OBJECTS:py_ymbase_obj_d>
  $<TARGET_OBJECTS:py_ymcell_obj_d>
  $<TARGET_OBJECTS:py_ymsat_obj_d>
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( py_druid_d
  PRIVATE "-g"
  )

target_include_directories ( py_druid_d
  PRIVATE
  ${PYTHON_INCLUDE_DIR}
  )

target_link_libraries ( py_druid_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install ( TARGETS py_druid DESTINATION bin )
install ( TARGETS py_druid_d DESTINATION bin )