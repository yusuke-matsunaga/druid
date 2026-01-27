
/// @file py_druid.cc
/// @brief py_druid の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/ymworks.h"


BEGIN_NAMESPACE_DRUID

extern "C"
PyObject* PyInit_druid();

END_NAMESPACE_DRUID

int
main(
  int argc,
  char** argv
)
{
  // "ymworks" モジュールを予め組み込んでおく．
  PyImport_AppendInittab("ymworks", YM_NAMESPACE::PyInit_ymworks);

  // "druid" モジュールを予め組み込んでおく．
  PyImport_AppendInittab("druid", DRUID_NAMESPACE::PyInit_druid);

  return Py_BytesMain(argc, argv);
}
