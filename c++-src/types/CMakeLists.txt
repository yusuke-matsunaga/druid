
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

set (types_SOURCES
  tpg/TpgBase.cc
  tpg/TpgNetwork.cc
  tpg/TpgGate.cc
  tpg/TpgNode.cc
  tpg/TpgFFR.cc
  tpg/TpgMFFC.cc
  tpg/TpgFault.cc
  tpg/AssignList.cc

  network/NetworkRep.cc
  network/NetworkRep_make_node.cc
  network/NetworkRep_gen_fault.cc
  network/NetworkRep_tfotfi.cc
  network/NetBuilder.cc
  network/NetBuilder_bn.cc
  network/DfsDriver.cc

  gate/GateRep.cc

  gatetype/GateType.cc
  gatetype/GateType_PPI.cc
  gatetype/GateType_PPO.cc
  gatetype/GateType_Primitive.cc
  gatetype/GateType_Cplx.cc
  #gatetype/GateTypeMgr.cc

  node/NodeRep.cc
  node/Node_PPI.cc
  node/Node_PPO.cc
  node/Node_Logic.cc

  fault/FaultRep.cc
  fault/Fault_Stem.cc
  fault/Fault_Branch.cc
  fault/Fault_Ex.cc

  tvect/BitVector.cc
  tvect/BitVectorRep.cc
  tvect/TestVector.cc
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
