
/// @file dtpg_module.cc
/// @brief dtpg_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "pym/PyDtpgMgr.h"
#include "pym/PyDtpgResult.h"
#include "pym/PyDtpgStats.h"
#include "pym/PyStructEngine.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef dtpg_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef dtpg_module = {
  PyModuleDef_HEAD_INIT,
  "dtpg",
  PyDoc_STR("dtpg: a module for type definition"),
  -1,
  dtpg_methods,
};

PyMODINIT_FUNC
PyInit_dtpg()
{
  auto m = PyModule::init(&dtpg_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyDtpgMgr::init(m) ) {
    goto error;
  }

  if ( !PyDtpgResult::init(m) ) {
    goto error;
  }

  if ( !PyDtpgStats::init(m) ) {
    goto error;
  }

  if ( !PyStructEngine::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NONAMESPACE

bool
dtpg_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "dtpg", PyInit_dtpg());
}

END_NAMESPACE_DRUID
