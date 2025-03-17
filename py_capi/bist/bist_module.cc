
/// @file bist_module.cc
/// @brief bist_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyPhaseShifter.h"
#include "pym/PyLFSR.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef bist_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef bist_module = {
  PyModuleDef_HEAD_INIT,
  "bist",
  PyDoc_STR("bist: a module for BIST(built-in self test)"),
  -1,
  bist_methods,
};

PyMODINIT_FUNC
PyInit_bist()
{
  auto m = PyModule::init(&bist_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyPhaseShifter::init(m) ) {
    goto error;
  }
  if ( !PyLFSR::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NONAMESPACE

bool
bist_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "bist", PyInit_bist());
}

END_NAMESPACE_DRUID
