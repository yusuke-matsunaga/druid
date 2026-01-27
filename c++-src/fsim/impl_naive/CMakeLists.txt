
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  )

# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (fsim_SOURCES
  EventQ.cc
  FsimX.cc
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

ym_add_object_library ( druid_fsim_naive_combi2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsim_naive_combi2_obj
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsim_naive_combi2_obj_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsim_naive_combi2_obj_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )


ym_add_object_library ( druid_fsim_naive_combi3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsim_naive_combi3_obj
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsim_naive_combi3_obj_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsim_naive_combi3_obj_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )


ym_add_object_library ( druid_fsim_naive_bside2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsim_naive_bside2_obj
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsim_naive_bside2_obj_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsim_naive_bside2_obj_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )


ym_add_object_library ( druid_fsim_naive_bside3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsim_naive_bside3_obj
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsim_naive_bside3_obj_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsim_naive_bside3_obj_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
