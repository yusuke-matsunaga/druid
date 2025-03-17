
/// @file condgen_module.cc
/// @brief condgen_module の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "CondGenMgr.h"
#include "DetCond.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFault.h"
#include "pym/PyStructEngine.h"
#include "pym/PySatLiteral.h"
#include "pym/PyDetCond.h"
#include "pym/PyCnfSize.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

PyObject*
make_cond(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "option",
    nullptr
  };

  PyObject* network_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::_get_ref(network_obj);
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }

  auto cond_list = CondGenMgr::make_cond(network, option);

  auto ans_obj = PyDetCond::ToPyList(cond_list);
  return ans_obj;
}

PyObject*
make_cnf(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "engine",
    "cond_list",
    "option",
    nullptr
  };
  PyObject* engine_obj = nullptr;
  PyObject* cond_list_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O|O",
				    const_cast<char**>(kw_list),
				    PyStructEngine::_typeobject(), &engine_obj,
				    &cond_list_obj,
				    &option_obj) ) {
    return nullptr;
  }
  auto& engine = PyStructEngine::_get_ref(engine_obj);
  std::vector<DetCond> cond_list;
  if ( !PyDetCond::FromPyList(cond_list_obj, cond_list) ) {
    PyErr_SetString(PyExc_TypeError, "'cond_list' should be a list of DetCond'");
    return nullptr;
  }
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }

  auto lits_list = CondGenMgr::make_cnf(engine, cond_list, option);

  auto n = lits_list.size();
  auto ans_obj = PyList_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto& lits = lits_list[i];
    auto m = lits.size();
    auto lits_obj = PyList_New(m);
    for ( SizeType j = 0; j < m; ++ j ) {
      auto lit = lits[j];
      auto lit_obj = PySatLiteral::ToPyObject(lit);
      PyList_SetItem(lits_obj, j, lit_obj);
    }
    PyList_SetItem(ans_obj, i, lits_obj);
  }

  return ans_obj;
}

// メソッド定義構造体
PyMethodDef condgen_methods[] = {
  {"make_cond",
   reinterpret_cast<PyCFunction>(make_cond),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate propagate condition of the roots of FFRs")},
  {"make_cnf",
   reinterpret_cast<PyCFunction>(make_cnf),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("make CNF for propagate condition of the roots of FFRs")},
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef condgen_module = {
  PyModuleDef_HEAD_INIT,
  "condgen",
  PyDoc_STR("condgen: a module for Condition Generator"),
  -1,
  condgen_methods,
};

PyMODINIT_FUNC
PyInit_condgen()
{
  auto m = PyModule::init(&condgen_module);
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
condgen_init(
  PyObject* m_parent
)
{
  return PyModule::reg_submodule(m_parent, "condgen", PyInit_condgen());
}

END_NAMESPACE_DRUID
