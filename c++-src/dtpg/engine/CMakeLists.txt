
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  .
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( gtest )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( engine_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/GateEnc.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/StructEngine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/BdEngine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Bd2Engine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FFR2Engine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DtpgEngine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/NaiveDtpgEngine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/NaiveDualEngine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/BoolDiffEnc.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FaultEnc_Node.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FaultEnc_FFR.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FaultEnc_MFFC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MFFCEnc.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FaultInfo.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FaultAnalyze.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/FFRAnalyze.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/MFFCAnalyze.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/DomChecker.cc
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
