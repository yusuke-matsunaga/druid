
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
#include "pym/PyTestCond.h"
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
    "callback",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  SizeType limit = 0;
  PyObject* callback_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!kO|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &limit,
				    &callback_obj,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  if ( !PyCallable_Check(callback_obj) ) {
    PyErr_SetString(PyExc_TypeError, "'callback' should be a callable type");
    return nullptr;
  }
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }
  CondGenMgr::root_cond(network, limit,
			[&](const TpgFFR* ffr,
			    const TestCond& cond,
			    SizeType count,
			    double time){
			  auto ffr_obj = PyTpgFFR::ToPyObject(ffr);
			  auto cond_obj = PyTestCond::ToPyObject(cond);
			  auto ret_obj = PyObject_CallFunction(callback_obj,
							       "(OOkd)",
							       ffr_obj,
							       cond_obj,
							       count,
							       time);
			  Py_DECREF(ffr_obj);
			  Py_DECREF(cond_obj);
			  Py_XDECREF(ret_obj);
			  // 本当は ret_obj == nullptr の時のエラー処理を
			  // しなければならない．
			  // たぶん，ここで例外を送出して func_cond() の外側
			  // で catch するのが正解
			}, option);
  Py_RETURN_NONE;
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
    "callback",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  SizeType limit = 0;
  PyObject* callback_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!OkO|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &fault_list_obj,
				    &limit,
				    &callback_obj,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    PyErr_SetString(PyExc_TypeError, "'fault_list' should be a list of TpgFault");
    return nullptr;
  }
  if ( !PyCallable_Check(callback_obj) ) {
    PyErr_SetString(PyExc_TypeError, "'callback' should be a callable type");
    return nullptr;
  }
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_TypeError, "'option' should be a JsonValue type");
    return nullptr;
  }
  CondGenMgr::fault_cond(network, fault_list, limit,
			 [&](const TpgFault* fault,
			     const TestCond& cond,
			     SizeType count,
			     double time){
			   auto fault_obj = PyTpgFault::ToPyObject(fault);
			   auto cond_obj = PyTestCond::ToPyObject(cond);
			   auto ret_obj = PyObject_CallFunction(callback_obj,
								"(OOkd)",
								fault_obj,
								cond_obj,
								count,
								time);
			   Py_DECREF(fault_obj);
			   Py_DECREF(cond_obj);
			   Py_XDECREF(ret_obj);
			   // 本当は ret_obj == nullptr の時のエラー処理を
			   // しなければならない．
			   // たぶん，ここで例外を送出して func_cond() の外側
			   // で catch するのが正解
			 }, option);
  Py_RETURN_NONE;
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

  if ( !PyTestCond::init(m) ) {
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
