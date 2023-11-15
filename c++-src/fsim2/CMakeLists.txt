
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/main
  )

# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (fsim2_SOURCES
  EventQ.cc
  FsimX.cc
  InputVals.cc
  SimNode.cc
  SnAnd.cc
  SnBuff.cc
  SnGate.cc
  SnInput.cc
  SnOr.cc
  SnXor.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================

ym_add_object_library ( druid_fsim2_main
  Fsim2.cc
  )

ym_add_object_library ( druid_fsim2combi2
  ${fsim2_SOURCES}
  )

target_compile_definitions ( druid_fsim2combi2_obj
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_COMBI=1"
  )

target_compile_definitions ( druid_fsim2combi2_obj_d
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_COMBI=1"
  )

target_compile_definitions ( druid_fsim2combi2_obj_p
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_COMBI=1"
  )


ym_add_object_library ( druid_fsim2combi3
  ${fsim2_SOURCES}
  )

target_compile_definitions ( druid_fsim2combi3_obj
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_COMBI=1"
  )

target_compile_definitions ( druid_fsim2combi3_obj_d
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_COMBI=1"
  )

target_compile_definitions ( druid_fsim2combi3_obj_p
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_COMBI=1"
  )


ym_add_object_library ( druid_fsim2bside2
  ${fsim2_SOURCES}
  )

target_compile_definitions ( druid_fsim2bside2_obj
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_BSIDE=1"
  )

target_compile_definitions ( druid_fsim2bside2_obj_d
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_BSIDE=1"
  )

target_compile_definitions ( druid_fsim2bside2_obj_p
  PRIVATE "-DFSIM2_VAL2=1" "-DFSIM2_BSIDE=1"
  )


ym_add_object_library ( druid_fsim2bside3
  ${fsim2_SOURCES}
  )

target_compile_definitions ( druid_fsim2bside3_obj
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_BSIDE=1"
  )

target_compile_definitions ( druid_fsim2bside3_obj_d
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_BSIDE=1"
  )

target_compile_definitions ( druid_fsim2bside3_obj_p
  PRIVATE "-DFSIM2_VAL3=1" "-DFSIM2_BSIDE=1"
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
