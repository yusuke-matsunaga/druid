
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
  driver/NodeEngineDriver.cc
  driver/FFRDriver.cc
  driver/FFREngineDriver.cc
  driver/FFRStructEncDriver.cc
  driver/FFREncDriver.cc
  driver/MFFCDriver.cc
  driver/MFFCEncDriver.cc
  driver/MFFCEngineDriver.cc
  driver/MFFCStructEncDriver.cc
  )

set ( engine_SOURCES
  engine/BaseEnc.cc
  engine/BoolDiffEnc.cc
  engine/FFREnc.cc
  engine/MFFCEnc.cc
  engine/FaultEnc.cc
  engine/BoolDiffEngine.cc
  engine/DtpgEngine.cc
  engine/DtpgEngine_Node.cc
  engine/DtpgEngine_FFR.cc
  engine/DtpgEngine_MFFC.cc
  engine/ConeEnc.cc
  engine/BSEnc.cc
  )

set ( struct_enc_SOURCES
  struct_enc/GateEnc.cc
  #struct_enc/FaultyGateEnc.cc
  struct_enc/PropCone.cc
  struct_enc/SimplePropCone.cc
  struct_enc/MffcPropCone.cc
  struct_enc/StructEnc.cc
  )

set ( ex_SOURCES
  ex/Extractor.cc
  ex/ExtSimple.cc
  ex/MultiExtractor.cc
  )

set (jt_SOURCES
  jt/Justifier.cc
  jt/JustImpl.cc
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
  ${struct_enc_SOURCES}
  ${ex_SOURCES}
  ${jt_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
