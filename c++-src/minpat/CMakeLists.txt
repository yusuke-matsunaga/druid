
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
  FFRFaultList.cc
  FaultAnalyzer.cc
  Reducer.cc
  XChecker.cc
  ffr_reduction.cc
  global_reduction.cc
  FFRDomChecker.cc
  DomCandMgr.cc
  DomCandGen.cc
  DomChecker.cc
  SimpleDomChecker.cc
  TrivialChecker1.cc
  TrivialChecker2.cc
  TrivialChecker3.cc
  NaiveDomChecker.cc
  NaiveDomChecker2.cc
  TestCoverGen.cc
  TestExprGen.cc
  ExCubeGen.cc
  ExprGen.cc
  ConflictChecker.cc
  FaultGroupGen.cc
  LocalImp.cc
  ColGraph.cc
  Dsatur.cc
  Isx.cc
  Sim.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_minpat
  ${minpat_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
