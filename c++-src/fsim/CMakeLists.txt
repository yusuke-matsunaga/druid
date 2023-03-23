
# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (fsim_SOURCES
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
ym_add_object_library ( druid_fsimcombi2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimcombi2_obj
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsimcombi2_obj_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsimcombi2_obj_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_COMBI=1"
  )


ym_add_object_library ( druid_fsimcombi3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimcombi3_obj
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsimcombi3_obj_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )

target_compile_definitions ( druid_fsimcombi3_obj_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_COMBI=1"
  )


ym_add_object_library ( druid_fsimbside2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimbside2_obj
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsimbside2_obj_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsimbside2_obj_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_BSIDE=1"
  )


ym_add_object_library ( druid_fsimbside3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimbside3_obj
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsimbside3_obj_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )

target_compile_definitions ( druid_fsimbside3_obj_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_BSIDE=1"
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
