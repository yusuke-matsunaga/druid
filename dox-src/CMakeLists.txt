

# ===================================================================
# doxygen 起動用のコマンド
# ===================================================================

if (DOXYGEN_FOUND)
  # doxygen の対象となるヘッダファイル(のパタン)を YM_DOXY_DIRS に入れる．
  set ( YM_DOXY_DIRS
    "${PROJECT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../py_capi/include"
    )

  # YM_DOXY_DIRS の内容を空白で区切られた文字列に変換する．
  set ( YM_DOXY_INPUTS "" )
  foreach ( _ym_arg ${YM_DOXY_DIRS} )
    set ( YM_DOXY_INPUTS "${YM_DOXY_INPUTS} ${_ym_arg}" )
  endforeach ()
  set ( YM_DOXY_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
  set ( YM_DOXY_HTML_FILE ${YM_DOXY_OUTPUT_DIR}/html/index.html )
  set ( YM_DOXY_XML_FILE ${YM_DOXY_OUTPUT_DIR}/xml/index.xml )
  set ( YM_DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )

  # Doxyfile の生成
  configure_file (
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${YM_DOXYFILE}
    @ONLY
    )

  # 対象のヘッダーファイル
  set ( YM_DOXY_SRCS  )
  foreach ( _ym_arg ${YM_DOXY_DIRS} )
    file ( GLOB_RECURSE _ym_headers "${_ym_arg}/*.h" )
    list ( APPEND YM_DOXY_SRCS ${_ym_headers} )
  endforeach()

  # doxygen 用ターゲット
  add_custom_target(Doxygen_tgt ALL
    DEPENDS ${YM_DOXY_HTML_FILE} ${YM_DOXY_XML_FILE}
    )

  cmake_policy( SET CMP0058 NEW )

  # doxygen 起動ルール
  add_custom_command(
    COMMAND "${DOXYGEN_EXECUTABLE}" ${YM_DOXYFILE} ">" "doxygen.log"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${YM_DOXYFILE}
    DEPENDS ${YM_DOXY_SRCS}
    OUTPUT ${YM_DOXY_HTML_FILE} ${YM_DOXY_XML_FILE}
    COMMENT "generating doxygen documents"
    )

endif (DOXYGEN_FOUND)
