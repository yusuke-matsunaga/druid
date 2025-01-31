
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
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyTpgFault.h"
#include "pym/PySatLiteral.h"
#include "pym/PyCnfSize.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

PyObject*
make_ffr_cond(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "limit",
    "option",
    nullptr
  };
  PyObject* engine_obj = nullptr;
  PyObject* network_obj = nullptr;
  SizeType limit = 0;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!k|O",
				    const_cast<char**>(kw_list),
				    PyStructEngine::_typeobject(), &network_obj,
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &limit,
				    &option_obj) ) {
    return nullptr;
  }
  auto& engine = PyStructEngine::Get(engine_obj);
  auto& network = PyTpgNetwork::Get(network_obj);
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }
  auto lits_list = CondGenMgr::make_ffr_cond(engine, network, limit, option);

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

PyObject*
calc_ffr_cond_size(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "limit",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  SizeType limit = 0;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!k|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &limit,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }
  auto size = CondGenMgr::calc_ffr_cond_size(network, limit, option);

  return PyCnfSize::ToPyObject(size);
}

// メソッド定義構造体
PyMethodDef condgen_methods[] = {
  {"make_ffr_cond",
   reinterpret_cast<PyCFunction>(make_ffr_cond),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate propagate condition of the roots of FFRs")},
  {"calc_ffr_cond_size",
   reinterpret_cast<PyCFunction>(calc_ffr_cond_size),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("calculate CNF size for propagate condition of the roots of FFRs")},
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
