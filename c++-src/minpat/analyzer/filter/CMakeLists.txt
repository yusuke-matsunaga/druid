
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( gtest )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( minpat_filter_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/PatGen.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PGImpl.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/EqGroupMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/EqGroupGraph.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/NaiveMgr.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DichoMgr.cc
  #${CMAKE_CURRENT_SOURCE_DIR}/DichoMgr2.cc
  #${CMAKE_CURRENT_SOURCE_DIR}/DCM2Helper.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DPat.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DPatGraph.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DomMgr.cc
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
