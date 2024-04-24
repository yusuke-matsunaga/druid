
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  ${PROJECT_SOURCE_DIR}/include/fsim
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( gtest )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( dtpg_SOURCES
  engine/DtpgEngine.cc
  engine/FFREngine.cc
  engine/MFFCEngine.cc
  engine/ConeEnc.cc
  engine/BSEnc.cc
  )

set ( driver_SOURCES
  driver/DtpgDriver.cc
  driver/FFRDriver.cc
  driver/FFREngineDriver.cc
  driver/FFRStructEncDriver.cc
  driver/MFFCDriver.cc
  driver/MFFCEngineDriver.cc
  driver/MFFCStructEncDriver.cc
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
  jt/Just1.cc
  jt/Just2.cc
  )

set (dtpg_mgr_SOURCES
  dtpg_mgr/DtpgMgr.cc
  #dtpg_mgr/DtpgDriver.cc
  #dtpg_mgr/DtpgEngineDriver.cc
  #dtpg_mgr/StructEncDriver.cc
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


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_dtpg
  ${dtpg_SOURCES}
  ${driver_SOURCES}
  ${struct_enc_SOURCES}
  ${ex_SOURCES}
  ${jt_SOURCES}
  ${dtpg_mgr_SOURCES}
  #${dop_SOURCES}
  #${uop_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
