
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/fsim
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

set ( mgr_SOURCES
  dtpg_mgr/DtpgMgr.cc
  )

set ( driver_SOURCES
  driver/DtpgDriver.cc
  driver/NodeDriver.cc
  driver/NodeEncDriver.cc
  driver/FFRDriver.cc
  driver/FFREncDriver.cc
  driver/MFFCDriver.cc
  driver/MFFCEncDriver.cc
  )

set ( engine_SOURCES
  engine/GateEnc.cc
  engine/StructEngine.cc
  engine/BoolDiffEnc.cc
  engine/BoolDiffEnc2.cc
  engine/FFREnc.cc
  engine/MFFCEnc.cc
  engine/FaultEnc.cc
  )

set ( ex_SOURCES
  ex/Extractor.cc
  ex/Extractor2.cc
  ex/ExtSimple.cc
  ex/MultiExtractor.cc
  ex/ExData.cc
  )

set (jt_SOURCES
  jt/Justifier.cc
  jt/JustNaive.cc
  jt/JustBase.cc
  jt/Just1.cc
  jt/Just2.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_dtpg
  ${mgr_SOURCES}
  ${driver_SOURCES}
  ${engine_SOURCES}
  ${ex_SOURCES}
  ${jt_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
