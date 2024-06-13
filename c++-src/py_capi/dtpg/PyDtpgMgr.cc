
/// @file PyDtpgMgr.cc
/// @brief Python DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgMgr.h"
#include "pym/PyModule.h"
#include "PyTpgNetwork.h"
#include "PyTpgFault.h"
#include "PyTestVector.h"
#include "PyDtpgStats.h"
#include "pym/PyJsonValue.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgMgrObject
{
  PyObject_HEAD
  DtpgMgr* mVal;
};

// Python 用のタイプ定義
PyTypeObject DtpgMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DtpgMgr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "network",
    "fault_list",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    &fault_list_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  mgr_obj->mVal = new DtpgMgr{network, fault_list};
  return self;
}

// 終了関数
void
DtpgMgr_dealloc(
  PyObject* self
)
{
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  delete mgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgMgr_run(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "det_func",
    "untest_func",
    "abort_func",
    "option",
    nullptr
  };
  PyObject* option_obj = nullptr;
  PyObject* dfunc_obj = nullptr;
  PyObject* ufunc_obj = nullptr;
  PyObject* afunc_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OOO|O",
				    const_cast<char**>(kw_list),
				    &dfunc_obj, &ufunc_obj, &afunc_obj,
				    &option_obj) ) {
    return nullptr;
  }
  if ( !PyCallable_Check(dfunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "3rd argument must be callable");
    return nullptr;
  }
  if ( !PyCallable_Check(ufunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "4th argument must be callable");
    return nullptr;
  }
  if ( !PyCallable_Check(afunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "5th argument must be callable");
    return nullptr;
  }
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_ValueError, "illegal value for option");
    return nullptr;
  }
  auto& mgr = PyDtpgMgr::Get(self);
  auto stats = mgr.run(
    [&](DtpgMgr& mgr, const TpgFault* f, const TestVector& tv) {
      auto f_obj = PyTpgFault::ToPyObject(f);
      auto tv_obj = PyTestVector::ToPyObject(tv);
      auto args = Py_BuildValue("(OO)", f_obj, tv_obj);
      auto ans_obj = PyObject_CallObject(dfunc_obj, args);
      Py_DECREF(ans_obj);
      Py_DECREF(args);
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
      auto f_obj = PyTpgFault::ToPyObject(f);
      auto ans_obj = PyObject_CallObject(ufunc_obj, f_obj);
      Py_DECREF(ans_obj);
      Py_DECREF(f_obj);
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
      auto f_obj = PyTpgFault::ToPyObject(f);
      auto ans_obj = PyObject_CallObject(afunc_obj, f_obj);
      Py_DECREF(ans_obj);
      Py_DECREF(f_obj);
    },
    option);
  return PyDtpgStats::ToPyObject(stats);
}

// メソッド定義
PyMethodDef DtpgMgr_methods[] = {
  {"run", reinterpret_cast<PyCFunction>(DtpgMgr_run),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("run")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DtpgMgr' オブジェクトを使用可能にする．
bool
PyDtpgMgr::init(
  PyObject* m
)
{
  DtpgMgrType.tp_name = "DtpgMgr";
  DtpgMgrType.tp_basicsize = sizeof(DtpgMgrObject);
  DtpgMgrType.tp_itemsize = 0;
  DtpgMgrType.tp_dealloc = DtpgMgr_dealloc;
  DtpgMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  DtpgMgrType.tp_doc = PyDoc_STR("DtpgMgr object");
  DtpgMgrType.tp_methods = DtpgMgr_methods;
  DtpgMgrType.tp_new = DtpgMgr_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DtpgMgr", &DtpgMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が DtpgMgr タイプか調べる．
bool
PyDtpgMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DtpgMgr を表す PyObject から DtpgMgr を取り出す．
DtpgMgr&
PyDtpgMgr::Get(
  PyObject* obj
)
{
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(obj);
  return *mgr_obj->mVal;
}

// @brief DtpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMgr::_typeobject()
{
  return &DtpgMgrType;
}

END_NAMESPACE_DRUID
