# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.21)


# C++17 の指定
set (CMAKE_CXX_STANDARD 17)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
set ( YM_VERSION_MAJOR 1 )
set ( YM_VERSION_MINOR 0 )
set ( YM_VERSION "${YM_VERSION_MAJOR}.${YM_VERSION_MINOR}" )
project ( druid VERSION "${YM_VERSION}" )

set ( YM_PROJECT_STRING "${PROJECT_NAME}-${PROJECT_VERSION}" )

# 名前空間
set ( DRUID_NSNAME "nsDruid" )

# インストールディレクトリ
if ( INSTALL_PREFIX )
  set ( CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX} )
else ()
  set ( CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/stage" )
endif ()


# ===================================================================
# オプション
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================
list ( APPEND CMAKE_MODULE_PATH
  "${PROJECT_SOURCE_DIR}/ym-common/cmake"
  )

include (YmUtils)

if ( PYTHON_VERSION )
  set( INIT_PYTHON_ARGS ${PYTHON_VERSION} EXACT )
endif ()

ym_init ( "" )
ym_init_python ( ${INIT_PYTHON_ARGS} )
#ym_init_doxygen ()
ym_init_gperftools ()
ym_init_ctest ()

# Python パッケージのインストール先
set ( YM_DRUID_PKGDIR "${YM_PYTHON_LIBDIR}/${PROJECT_NAME}" )

list ( APPEND YM_LIB_DEPENDS pthread )


# ===================================================================
# google-test は内蔵のものを使う．
# ===================================================================
ym_use_gtest ()


# ===================================================================
# ヘッダファイルの生成
# ===================================================================

configure_file(
  "${PROJECT_SOURCE_DIR}/etc/druid.h.in"
  "${PROJECT_BINARY_DIR}/include/druid.h"
  )


# ===================================================================
# スクリプトファイルの生成
# ===================================================================


# ===================================================================
# サブモジュールのリスト
# ===================================================================

ym_init_submodules (
  ym-base
  ym-bn
  ym-cell
  ym-combopt
  ym-common
  ym-logic
  ym-sat
  )


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}/include
  ${YM_SUBMODULE_INCLUDE_LIST}
  )


# ===================================================================
# テスト用データのディレクトリ
# ===================================================================
set ( TESTDATA_DIR ${PROJECT_SOURCE_DIR}/tests/data )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

foreach ( dir ${YM_SUBMODULE_LIST} )
  add_subdirectory ( ${dir} )
endforeach ()

add_subdirectory ( c++-src )
add_subdirectory ( py_capi )

if ( DOXYGEN_FOUND )
  # doxygen の対象となるヘッダファイル(のパタン)を YM_DOXY_SRCS に入れる．
  set ( YM_DOXY_SRCS
    ${PROJECT_SOURCE_DIR}/etc/mainpage.h
    ${PROJECT_BINARY_DIR}/include
    ${PROJECT_SOURCE_DIR}/include
    ${YM_SUBMODULE_INCLUDE_LIST}
    )

  ym_add_doxygen ()
endif ()


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
