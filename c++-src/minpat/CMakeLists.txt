
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/fsim
  include
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( gtest )
add_subdirectory( test-programs )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( minpat_SOURCES
  FaultInfoMgr.cc
  FaultAnalyzer.cc
  FFRFaultList.cc
  TestCoverGen.cc
  TestExprGen.cc
  ExCubeGen.cc
  CondGen.cc
  ExprGen.cc
  ConflictChecker.cc
  FaultGroupGen.cc
  LocalImp.cc
  ColGraph.cc
  Dsatur.cc
  Isx.cc
  Sim.cc
  )

set ( reducer_SOURCES
  reducer/Reducer.cc
  reducer/SimpleDomChecker.cc
  reducer/TrivialChecker1.cc
  reducer/TrivialChecker2.cc
  reducer/TrivialChecker3.cc
  reducer/NaiveDomChecker.cc
  reducer/NaiveDomChecker2.cc
  reducer/ffr_reduction.cc
  reducer/global_reduction.cc
  reducer/FFRDomChecker.cc
  reducer/DomCandMgr.cc
  reducer/DomCandGen.cc
  reducer/DomChecker.cc
  reducer/XChecker.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_minpat
  ${minpat_SOURCES}
  ${reducer_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
