
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
ym_add_object_library ( druid_fsimsa2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimsa2
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa2_a
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa2_ad
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa2_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa2_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_SA=1"
  )


ym_add_object_library ( druid_fsimsa3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimsa3
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa3_a
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa3_ad
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa3_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_SA=1"
  )

target_compile_definitions ( druid_fsimsa3_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_SA=1"
  )


ym_add_object_library ( druid_fsimtd2
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimtd2
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd2_a
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd2_ad
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd2_d
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd2_p
  PRIVATE "-DFSIM_VAL2=1" "-DFSIM_TD=1"
  )


ym_add_object_library ( druid_fsimtd3
  ${fsim_SOURCES}
  )

target_compile_definitions ( druid_fsimtd3
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd3_a
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd3_ad
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd3_d
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_TD=1"
  )

target_compile_definitions ( druid_fsimtd3_p
  PRIVATE "-DFSIM_VAL3=1" "-DFSIM_TD=1"
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
