

# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================
set ( tpg_types_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/tpg_types_module.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyVal3.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyFval2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyFaultType.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyFaultStatus.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNetwork.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNode.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNodeList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgNodeIter2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgMFFC.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgMFFCList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgMFFCIter2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFFR.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFFRList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFFRIter2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgGate.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgGateList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgGateIter2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFault.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFaultList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTpgFaultIter2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyTestVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyInputVector.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyAssign.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyAssignList.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/PyAssignIter2.cc
  PARENT_SCOPE
  )


# ===================================================================
#  executable ターゲットの設定
# ===================================================================


# ===================================================================
# テストターゲット
# ===================================================================
