
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
  PyMt19937.cc
  PyVal3.cc
  PyFaultType.cc
  PyTestVector.cc
  PyInputVector.cc
  PyDffVector.cc
  PyTpgFault.cc
  PyTpgNetwork.cc
  PyFsim.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

add_executable ( py_druid
  ${py_druid_SOURCES}
  $<TARGET_OBJECTS:druid_common_obj>
  $<TARGET_OBJECTS:druid_fsimsa2_obj>
  $<TARGET_OBJECTS:druid_fsimsa3_obj>
  $<TARGET_OBJECTS:druid_fsimtd2_obj>
  $<TARGET_OBJECTS:druid_fsimtd3_obj>
  $<TARGET_OBJECTS:ym_luaclib_obj>
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
  $<TARGET_OBJECTS:druid_common_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa2_obj_d>
  $<TARGET_OBJECTS:druid_fsimsa3_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd2_obj_d>
  $<TARGET_OBJECTS:druid_fsimtd3_obj_d>
  $<TARGET_OBJECTS:ym_luaclib_obj_d>
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


if ( Readline_FOUND )
  foreach ( _tgt py_druid py_druid_d )
    target_include_directories ( ${_tgt}
      PRIVATE ${Readline_INCLUDE_DIR}
      )
    target_compile_definitions ( ${_tgt}
      PRIVATE "-DHAS_READLINE"
      )
  endforeach ()
endif ( Readline_FOUND )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
