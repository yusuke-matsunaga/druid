
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
  ${YM_SUBMODULE_INCLUDE_LIST}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( fsim )
add_subdirectory ( py_druid )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (tpg_network_SOURCES
  tpg_network/NodeValList.cc
  tpg_network/TpgNetwork.cc
  tpg_network/TpgNetworkImpl.cc
  tpg_network/TpgNetworkImpl_make_node.cc
  tpg_network/TpgDff.cc
  tpg_network/TpgGateInfo.cc
  tpg_network/TpgNode.cc
  tpg_network/TpgPPI.cc
  tpg_network/TpgPPO.cc
  tpg_network/TpgDffControl.cc
  tpg_network/TpgLogic.cc
  tpg_network/TpgFault.cc
  tpg_network/FaultStatusMgr.cc

  tvect/BitVectorRep.cc
  tvect/TestVector.cc

  )

set (fsim_SOURCES
  fsim/Fsim.cc
  )

set (struct_enc_SOURCES
  struct_enc/GateEnc.cc
  struct_enc/FaultyGateEnc.cc
  struct_enc/PropCone.cc
  struct_enc/SimplePropCone.cc
  struct_enc/MffcPropCone.cc
  struct_enc/StructEnc.cc
  )

set (dtpg_SOURCES
  dtpg/DtpgEngine.cc
  dtpg/DtpgFFR.cc
  dtpg/DtpgMFFC.cc
  dtpg/Dtpg_se.cc
  )

set (dop_SOURCES
  dop/DopBase.cc
  dop/DopDrop.cc
  dop/DopDummy.cc
  dop/DopList.cc
  dop/DopTvList.cc
  dop/DopVerify.cc
  dop/DopVerifyResult.cc
  )

set (uop_SOURCES
  uop/UopBase.cc
  uop/UopDummy.cc
  uop/UopList.cc
  uop/UopSkip.cc
  )

set (ex_SOURCES
  ex/Extractor.cc
  ex/MultiExtractor.cc
  )

set (jt_SOURCES
  jt/Justifier.cc
  jt/JustImpl.cc
  jt/Just1.cc
  jt/Just2.cc
  )

set (colcov_SOURCES
  colcov/ColCov.cc
  )

set (minpat_SOURCES
  minpat/MinPatMgr.cc
  minpat/MpColGraph.cc
  minpat/MatrixGen.cc
  minpat/Analyzer.cc
  minpat/FaultReducer.cc
  minpat/UndetChecker.cc
  minpat/DomChecker.cc
  minpat/TvMerger.cc
  )

set (mf_SOURCES
  mf_dtpg/MF_Dtpg.cc
  mf_dtpg/MF_Enc.cc
  mf_dtpg/MF_FaultComp.cc
  )

set (sa_SOURCES

  sa/minpat/Verifier.cc
  sa/minpat/MinPatBase.cc
  sa/minpat/MinPatNaive.cc
  sa/minpat/MinPatSimple.cc
  sa/minpat/MinPatSimple2.cc
  sa/minpat/MinPatDsatur.cc
  sa/minpat/MinPatDsatur2.cc
  sa/minpat/FaultAnalyzer.cc
  sa/minpat/EqChecker.cc
  sa/minpat/DomChecker.cc
  sa/minpat/ConflictChecker.cc
  sa/minpat/FgMgrBase.cc
  sa/minpat/Compactor.cc
  sa/minpat/EqSet.cc
  sa/minpat/McCompactor.cc

  sa/minpat/StructSat.cc
  sa/minpat/FoCone.cc
  sa/minpat/Extractor.cc

  sa/rtpg/RtpgImpl.cc
  )

set (td_SOURCES
  td/rtpg/RtpgP1.cc
  td/rtpg/RtpgP2.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_common
  ${tpg_network_SOURCES}
  ${fsim_SOURCES}
  ${struct_enc_SOURCES}
  ${dtpg_SOURCES}
  ${dop_SOURCES}
  ${uop_SOURCES}
  ${ex_SOURCES}
  ${jt_SOURCES}
  ${colcov_SOURCES}
  ${minpat_SOURCES}
  ${mf_SOURCES}
#  ${sa_SOURCES}
#  ${td_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
