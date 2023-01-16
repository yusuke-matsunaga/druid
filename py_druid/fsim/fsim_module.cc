
/// @file fsim_module.cc
/// @brief fsim_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "PyFsim.h"
#include "ym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef fsim_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef fsim_module = {
  PyModuleDef_HEAD_INIT,
  "fsim",
  PyDoc_STR("fsim: Fault simulator"),
  -1,
  fsim_methods,
};

PyMODINIT_FUNC
PyInit_fsim()
{
  auto m = PyModule::init(&fsim_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyFsim::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NONAMESPACE

bool
fsim_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "fsim", PyInit_fsim());
}

END_NAMESPACE_DRUID
