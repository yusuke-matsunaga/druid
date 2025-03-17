
/// @file types_module.cc
/// @brief types_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyVal3.h"
#include "pym/PyFaultType.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgMFFC.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyBitVector.h"
#include "pym/PyTestVector.h"
#include "pym/PyInputVector.h"
#include "pym/PyDffVector.h"
#include "pym/PyFaultStatus.h"
#include "pym/PyAssignExpr.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef types_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef types_module = {
  PyModuleDef_HEAD_INIT,
  "types",
  PyDoc_STR("types: a module for type definition"),
  -1,
  types_methods,
};

PyMODINIT_FUNC
PyInit_types()
{
  auto m = PyModule::init(&types_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyVal3::init(m) ) {
    goto error;
  }
  if ( !PyFaultType::init(m) ) {
    goto error;
  }
  if ( !PyFaultStatus::init(m) ) {
    goto error;
  }
  if ( !PyTpgFault::init(m) ) {
    goto error;
  }
  if ( !PyTpgNetwork::init(m) ) {
    goto error;
  }
  if ( !PyTpgMFFC::init(m) ) {
    goto error;
  }
  if ( !PyTpgFFR::init(m) ) {
    goto error;
  }
  if ( !PyTestVector::init(m) ) {
    goto error;
  }
  if ( !PyBitVector::init(m) ) {
    goto error;
  }
  if ( !PyInputVector::init(m) ) {
    goto error;
  }
  if ( !PyDffVector::init(m) ) {
    goto error;
  }
  if ( !PyAssignExpr::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NONAMESPACE

bool
types_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "types", PyInit_types());
}

END_NAMESPACE_DRUID
