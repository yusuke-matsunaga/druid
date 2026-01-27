
/// @file druid_module.cc
/// @brief Python 用の druid モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/tpg_types.h"
#include "pym/fsim.h"
#include "pym/dtpg.h"
#include "pym/condgen.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef druid_module = {
  PyModuleDef_HEAD_INIT,
  "druid",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_druid()
{
  auto m = PyModule::init(&druid_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyModule::reg_submodule(m, "tpg_types", PyInit_tpg_types()) ) {
    goto error;
  }
  if ( !PyModule::reg_submodule(m, "fsim", PyInit_fsim()) ) {
    goto error;
  }
  if ( !PyModule::reg_submodule(m, "dtpg", PyInit_dtpg()) ) {
    goto error;
  }
  if ( !PyModule::reg_submodule(m, "condgen", PyInit_condgen()) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
