
/// @file fsim_module.cc
/// @brief Python 用の fsim モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyDiffBits.h"
#include "pym/PyDiffBitsArray.h"
#include "pym/PyFsim.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef fsim_module = {
  PyModuleDef_HEAD_INIT,
  "fsim",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_fsim()
{
  auto m = PyModule::init(&fsim_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyDiffBits::init(m) ) {
    goto error;
  }
  if ( !PyDiffBitsArray::init(m) ) {
    goto error;
  }
  if ( !PyFsim::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
