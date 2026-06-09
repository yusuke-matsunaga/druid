
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
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
  MinPat.cc
  MpInit.cc
  MpReduce.cc
  MpComp.cc
  MpCompImpl.cc
  MpComp_MinCov.cc
  MpComp_Expand.cc
  MpComp_Pack.cc
  MpComp_Simple.cc
  MpComp_Merge.cc
  MpAnalyze.cc
  MpVerify.cc
  FaultAnalyze.cc
  FFRAnalyze.cc
  FaultInfo.cc
  MFFCAnalyze.cc
  DomChecker.cc
  #filter/Dichotomy.cc
  #filter/DiGroupMgr.cc
  #filter/NaiveReduce.cc
  #filter/NaiveMgr.cc
  filter/Reducer.cc
  filter/Filter.cc
  filter/EqDomCand.cc
  filter/PatGen.cc
  filter/CandMgr.cc
  filter/NaiveCandMgr.cc
  filter/DichoCandMgr.cc
  filter/DichoCandMgr2.cc
  filter/DPatGraph.cc
  filter/POSet.cc
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
