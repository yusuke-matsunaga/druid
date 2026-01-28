
# conf.py の生成
set ( YM_DOX_XML_DIR ${PROJECT_BINARY_DIR}/dox-src )
set ( DRUID_PATH ${PROJECT_BINARY_DIR}/py_capi )
configure_file (
  ${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py.in
  ${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py
  )