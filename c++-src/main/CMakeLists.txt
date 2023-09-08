
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/main
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (dtpg_mgr_SOURCES
  dtpg_mgr/DtpgMgr.cc
  dtpg_mgr/DtpgDriver.cc
  dtpg_mgr/DtpgEngineDriver.cc
  dtpg_mgr/StructEncDriver.cc
  )

set (dop_SOURCES
  dop/DopBase.cc
  dop/DopDrop.cc
  dop/DopDummy.cc
  dop/DopTvList.cc
  dop/DopVerify.cc
  dop/DopVerifyResult.cc
  )

set (uop_SOURCES
  uop/UopBase.cc
  uop/UopDummy.cc
  uop/UopSkip.cc
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
  minpat/DtpgEngine2.cc
  minpat/DtpgFFR.cc
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
ym_add_object_library ( druid_main
  ${dtpg_mgr_SOURCES}
  ${dop_SOURCES}
  ${uop_SOURCES}
  ${jt_SOURCES}
  ${colcov_SOURCES}
#  ${minpat_SOURCES}
#  ${mf_SOURCES}
#  ${sa_SOURCES}
#  ${td_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
