# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.2)


# C++14 の指定
set (CMAKE_CXX_STANDARD 14)


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
set ( CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/stage" )


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

# Python3 に関する初期化を行う．
#
# 入力: なし
# 出力: PYTHON_INTERP Python の実行ファイル
#       PYTHON_VERSION_MAJOR Python の major バージョン
#       PYTHON_VERSION_MINOR Python の minor バージョン
#       ... FindPythonInterp, FindPythonLibs によって設定される変数
#       YM_PYTHON_STRING: Python3.6 のような文字列
#       YM_PYTHON_LIBDIR: ここで生成される Python ライブラリの格納先
macro ( ym_init_python2 )
  # Python のバージョン指定
  find_package( Python3 ${ARGV} COMPONENTS Interpreter Development )
  set( YM_PYTHON_STRING "python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}" )
  set( YM_PYTHON_LIBDIR "lib/${YM_PYTHON_STRING}/site-packages/" )
endmacro ()

ym_init ( "" )
ym_init_python ( 3.9 EXACT )
ym_init_doxygen ()
ym_init_gperftools ()
ym_init_ctest ()

# Python パッケージのインストール先
set ( YM_DRUID_PKGDIR "${YM_PYTHON_LIBDIR}/${PROJECT_NAME}" )


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
  ym-bnet
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
# サブディレクトリの設定
# ===================================================================

foreach ( dir ${YM_SUBMODULE_LIST} )
  add_subdirectory ( ${dir} )
endforeach ()

add_subdirectory ( c++-src )
add_subdirectory ( py_druid )
add_subdirectory ( tests/common )
add_subdirectory ( tests/gtest )
add_subdirectory ( tests/py_druid )
add_subdirectory ( tests/test-programs )

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
