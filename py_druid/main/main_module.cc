
/// @file main_module.cc
/// @brief main_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

extern
PyObject*
classify(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
);

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef main_methods[] = {
  {"classify", reinterpret_cast<PyCFunction>(classify),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("classify")},
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef main_module = {
  PyModuleDef_HEAD_INIT,
  "main",
  PyDoc_STR("main: Main Module"),
  -1,
  main_methods,
};

PyMODINIT_FUNC
PyInit_main()
{
  auto m = PyModule::init(&main_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NONAMESPACE

bool
main_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "main", PyInit_main());
}

END_NAMESPACE_DRUID
