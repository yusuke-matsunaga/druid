
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/main
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/fsim
  ${PROJECT_SOURCE_DIR}/include/dtpg
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (colcov_SOURCES
  colcov/ColCov.cc
  )

set (classify_SOURCES
  classify/Classifier.cc
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


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_main
  ${colcov_SOURCES}
  ${classify_SOURCES}
#  ${minpat_SOURCES}
#  ${mf_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
