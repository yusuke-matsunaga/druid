
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

set ( minpat_filter_SOURCES
  #${CMAKE_CURRENT_SOURCE_DIR}/Dichotomy.cc
  #${CMAKE_CURRENT_SOURCE_DIR}/DiGroupMgr.cc
  #${CMAKE_CURRENT_SOURCE_DIR}/NaiveReduce.cc
  #${CMAKE_CURRENT_SOURCE_DIR}/NaiveMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Reducer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Filter.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/EqDomCand.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PatGen.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/CandMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/NaiveCandMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DichoCandMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DichoCandMgr2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DPatGraph.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/POSet.cc
  PARENT_SCOPE
  )


# ===================================================================
# ターゲットの設定
# ===================================================================


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
