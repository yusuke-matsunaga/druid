
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
#include "pym/PyDetCond.h"
#include "pym/PyJsonValue.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

PyObject*
root_cond(
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
  auto cond_list = CondGenMgr::root_cond(network, limit, option);

  return PyDetCond::ToPyList(cond_list);
}

PyObject*
fault_cond(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "fault_list",
    "limit",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  SizeType limit = 0;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!Ok|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &fault_list_obj,
				    &limit,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    PyErr_SetString(PyExc_TypeError, "'fault_list' should be a list of TpgFault");
    return nullptr;
  }
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }
  auto cond_list = CondGenMgr::fault_cond(network, fault_list, limit,
					  option);

  return PyDetCond::ToPyList(cond_list);
}

// メソッド定義構造体
PyMethodDef condgen_methods[] = {
  {"root_cond",
   reinterpret_cast<PyCFunction>(root_cond),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate propagate condition for the root of FFR")},
  {"fault_cond",
   reinterpret_cast<PyCFunction>(fault_cond),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate fault detect condition")},
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

  if ( !PyDetCond::init(m) ) {
    goto error;
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
