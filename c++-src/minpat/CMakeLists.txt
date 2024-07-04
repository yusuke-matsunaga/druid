
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

#add_subdirectory( gtest )
add_subdirectory( test-programs )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( minpat_SOURCES
  FaultReducer.cc
  FaultAnalyzer.cc
  FFRDomChecker.cc
  DomCandGen.cc
  DomChecker.cc
  DomChecker2.cc
  SimpleDomChecker.cc
  TrivialChecker1.cc
  TrivialChecker2.cc
  TrivialChecker3.cc
  TestCubeGen.cc
  NaiveDomChecker.cc
  NaiveDomChecker2.cc
  FFRFaultList.cc
  #StructDomChecker.cc
  #DcGraph.cc
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
