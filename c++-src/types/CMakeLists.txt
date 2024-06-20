
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( gtest )
add_subdirectory( test-programs )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (types_SOURCES
  tpg_network/GateType.cc
  tpg_network/NodeValList.cc
  tpg_network/NodeTimeValList.cc
  tpg_network/TpgNodeSet.cc
  tpg_network/TpgNetwork.cc
  tpg_network/TpgNetworkImpl.cc
  tpg_network/TpgNetworkImpl_bn.cc
  tpg_network/TpgNetworkImpl_make_node.cc
  tpg_network/TpgGate.cc
  tpg_network/TpgNode.cc
  tpg_network/TpgPPI.cc
  tpg_network/TpgPPO.cc
  tpg_network/TpgLogic.cc

  tvect/BitVector.cc
  tvect/BitVectorRep.cc
  tvect/TestVector.cc

  tpg_fault/TpgFault.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_types
  ${types_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
