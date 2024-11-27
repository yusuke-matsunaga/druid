
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/fsim
  ${PROJECT_SOURCE_DIR}/include/reducer
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

set ( reducer_SOURCES
  FaultInfoMgr.cc
  FaultAnalyzer.cc
  ConflictChecker.cc
  Reducer.cc
  SimpleDomChecker.cc
  TrivialChecker1.cc
  TrivialChecker2.cc
  TrivialChecker3.cc
  NaiveDomChecker.cc
  NaiveDomChecker2.cc
  ffr_reduction.cc
  global_reduction.cc
  FFRDomChecker.cc
  DomCandMgr.cc
  DomCandGen.cc
  DomChecker.cc
  XChecker.cc
  LocalImp.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_reducer
  ${reducer_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
