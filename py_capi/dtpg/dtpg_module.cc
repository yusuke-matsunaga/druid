
/// @file dtpg_module.cc
/// @brief Python 用の dtpg モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyStructEngine.h"
#include "pym/PyDtpgResult.h"
#include "pym/PyDtpgStats.h"
#include "pym/PyVidMap.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef dtpg_module = {
  PyModuleDef_HEAD_INIT,
  "dtpg",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_dtpg()
{
  auto m = PyModule::init(&dtpg_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyStructEngine::init(m) ) {
    goto error;
  }
  if ( !PyDtpgResult::init(m) ) {
    goto error;
  }
  if ( !PyDtpgStats::init(m) ) {
    goto error;
  }
  if ( !PyVidMap::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_DRUID
