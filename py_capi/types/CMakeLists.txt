
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================



# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( types_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/types_module.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyVal3.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyFaultType.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyFaultStatus.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyBitVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTestVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyInputVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyDffVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFault.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNetwork.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNode.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgMFFC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFFR.cc
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
