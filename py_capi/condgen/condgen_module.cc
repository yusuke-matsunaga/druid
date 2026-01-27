
/// @file condgen_module.cc
/// @brief Python 用の condgen モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyDetCond.h"
#include "pym/PyCondGenMgr.h"
#include "pym/PyCondGenStats.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef condgen_module = {
  PyModuleDef_HEAD_INIT,
  "condgen",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_condgen()
{
  auto m = PyModule::init(&condgen_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyDetCond::init(m) ) {
    goto error;
  }
  if ( !PyCondGenMgr::init(m) ) {
    goto error;
  }
  if ( !PyCondGenStats::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
