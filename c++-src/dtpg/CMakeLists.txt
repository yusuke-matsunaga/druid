
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include/types
  ${PROJECT_SOURCE_DIR}/include/dtpg
  #${PROJECT_SOURCE_DIR}/include/main
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (dtpg_SOURCES
  dtpg/DtpgEngine.cc
  dtpg/FFREngine.cc
  dtpg/MFFCEngine.cc
  )

set (struct_enc_SOURCES
  struct_enc/GateEnc.cc
  #struct_enc/FaultyGateEnc.cc
  struct_enc/PropCone.cc
  struct_enc/SimplePropCone.cc
  struct_enc/MffcPropCone.cc
  struct_enc/StructEnc.cc
  )

set (ex_SOURCES
  ex/Extractor.cc
  ex/ExtSimple.cc
  ex/MultiExtractor.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( druid_dtpg
  ${dtpg_SOURCES}
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
