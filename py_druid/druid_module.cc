
/// @file druid_module.cc
/// @brief Python 用の druid モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyModule.h"
#include "pym/ymbase.h"
#include "pym/ymcell.h"
#include "pym/ymsat.h"


BEGIN_NAMESPACE_DRUID

extern bool types_init(PyObject*);
extern bool fsim_init(PyObject*);
extern bool dtpg_init(PyObject*);
extern bool bist_init(PyObject*);
extern bool main_init(PyObject*);

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef druid_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef druid_module = {
  PyModuleDef_HEAD_INIT,
  "druid",
  PyDoc_STR("Druid: a framework for automatic test generation"),
  -1,
  druid_methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_druid()
{
  auto m = PyModule_Create(&druid_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyModule::reg_submodule(m, "ymbase", PyInit_ymbase()) ) {
    goto error;
  }
  if ( !PyModule::reg_submodule(m, "ymcell", PyInit_ymcell()) ) {
    goto error;
  }
  if ( !PyModule::reg_submodule(m, "ymsat", PyInit_ymsat()) ) {
    goto error;
  }

  if ( !types_init(m) ) {
    goto error;
  }
  if ( !dtpg_init(m) ) {
    goto error;
  }
  if ( !fsim_init(m) ) {
    goto error;
  }
  if ( !main_init(m) ) {
    goto error;
  }
  if ( !bist_init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
