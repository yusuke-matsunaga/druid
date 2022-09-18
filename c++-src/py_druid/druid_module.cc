
/// @file druid_module.cc
/// @brief Python 用の druid モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"


BEGIN_NAMESPACE_DRUID

extern bool PyInit_Mt19937(PyObject* m);
extern bool PyInit_Val3(PyObject* m);
extern bool PyInit_FaultType(PyObject* m);
extern bool PyInit_TpgFault(PyObject* m);
extern bool PyInit_TestVector(PyObject* m);
extern bool PyInit_InputVector(PyObject* m);
extern bool PyInit_DffVector(PyObject* m);
extern bool PyInit_TpgNetwork(PyObject* m);
extern bool PyInit_Fsim(PyObject* m);

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

  if ( !PyInit_Mt19937(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_Val3(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_FaultType(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_TpgFault(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_TpgNetwork(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_TestVector(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_InputVector(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_DffVector(m) ) {
    Py_DECREF(m);
    return nullptr;
  }
  if ( !PyInit_Fsim(m) ) {
    Py_DECREF(m);
    return nullptr;
  }

  return m;
}

END_NAMESPACE_DRUID
