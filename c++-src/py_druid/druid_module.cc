
/// @file druid_module.cc
/// @brief Python 用の druid モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "PyVal3.h"
#include "PyFaultType.h"
#include "PyTpgFault.h"
#include "PyTpgNetwork.h"
#include "PyTestVector.h"
#include "PyInputVector.h"
#include "PyDffVector.h"
#include "PyFsim.h"
#include "PyFaultStatus.h"
#include "PyDtpgResult.h"


BEGIN_NAMESPACE_YM

PyMODINIT_FUNC PyInit_ymbase();
PyMODINIT_FUNC PyInit_ymcell();

END_NAMESPACE_YM

BEGIN_NAMESPACE_DRUID

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

  if ( !PyInit_ymbase() ) {
    goto error;
  }
  if ( !PyInit_ymcell() ) {
    goto error;
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
  if ( !PyTestVector::init(m) ) {
    goto error;
  }
  if ( !PyInputVector::init(m) ) {
    goto error;
  }
  if ( !PyDffVector::init(m) ) {
    goto error;
  }
  if ( !PyFsim::init(m) ) {
    goto error;
  }
  if ( !PyDtpgResult::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
