
/// @file PyDtpgMgr.cc
/// @brief Python DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDtpgMgr.h"
#include "pym/PyModule.h"
#include "pym/PyTpgNetwork.h"
#include "pym/PyTpgFault.h"
#include "pym/PyTestVector.h"
#include "pym/PyDtpgResult.h"
#include "pym/PyDtpgStats.h"
#include "pym/PyJsonValue.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DtpgMgrObject
{
  PyObject_HEAD
  DtpgMgr mVal;
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
  auto& network = PyTpgNetwork::_get_ref(network_obj);
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  new (&mgr_obj->mVal) DtpgMgr{network, fault_list};
  return self;
}

// 終了関数
void
DtpgMgr_dealloc(
  PyObject* self
)
{
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  mgr_obj->mVal.~DtpgMgr();
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

  PyObject* dfunc_obj = nullptr;
  PyObject* ufunc_obj = nullptr;
  PyObject* afunc_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|OOOO",
				    const_cast<char**>(kw_list),
				    &dfunc_obj, &ufunc_obj, &afunc_obj,
				    &option_obj) ) {
    return nullptr;
  }
  if ( dfunc_obj != nullptr && !PyCallable_Check(dfunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument(det_func) must be callable");
    return nullptr;
  }
  if ( ufunc_obj != nullptr && !PyCallable_Check(ufunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "2nd argument(untest_func) must be callable");
    return nullptr;
  }
  if ( afunc_obj != nullptr && !PyCallable_Check(afunc_obj) ) {
    PyErr_SetString(PyExc_TypeError, "3rd argument(abort_func) must be callable");
    return nullptr;
  }
  JsonValue option;
  PyJsonValue::Deconv json_dec;
  if ( !json_dec(option_obj, option) ) {
    PyErr_SetString(PyExc_ValueError, "illegal value for option");
    return nullptr;
  }

  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto stats = mgr.run(
    [&](DtpgMgr& mgr, const TpgFault* f, const TestVector& tv) {
      if ( dfunc_obj != nullptr ) {
	auto f_obj = PyTpgFault::ToPyObject(f);
	auto tv_obj = PyTestVector::ToPyObject(tv);
	Py_INCREF(self);
	auto args = Py_BuildValue("(OOO)", self, f_obj, tv_obj);
	auto ret_obj = PyObject_CallObject(dfunc_obj, args);
	Py_DECREF(self);
	Py_DECREF(args);
	Py_DECREF(ret_obj);
      }
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
      if ( ufunc_obj != nullptr ) {
	auto f_obj = PyTpgFault::ToPyObject(f);
	Py_INCREF(self);
	auto args = Py_BuildValue("(OO)", self, f_obj);
	auto ret_obj = PyObject_CallObject(ufunc_obj, args);
	Py_DECREF(self);
	Py_DECREF(args);
	Py_DECREF(ret_obj);
      }
    },
    [&](DtpgMgr& mgr, const TpgFault* f) {
      if ( afunc_obj != nullptr ) {
	auto f_obj = PyTpgFault::ToPyObject(f);
	Py_INCREF(self);
	auto args = Py_BuildValue("(OO)", self, f_obj);
	auto ret_obj = PyObject_CallObject(afunc_obj, args);
	Py_DECREF(self);
	Py_DECREF(args);
	Py_DECREF(ret_obj);
      }
    },
    option);
  return PyDtpgStats::ToPyObject(stats);
}

PyObject*
DtpgMgr_dtpg_result(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kw_list[] = {
    "fault",
    nullptr
  };
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
				    const_cast<char**>(kw_list),
				    PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::_get(fault_obj);
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto result = mgr.dtpg_result(fault);
  return PyDtpgResult::ToPyObject(result);
}

// メソッド定義
PyMethodDef DtpgMgr_methods[] = {
  {"run", reinterpret_cast<PyCFunction>(DtpgMgr_run),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("run")},
  {"dtpg_result", reinterpret_cast<PyCFunction>(DtpgMgr_dtpg_result),
   METH_KEYWORDS | METH_VARARGS,
   PyDoc_STR("get dtpg_result")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DtpgMgr_fault_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto& fault_list = mgr.fault_list();
  return PyTpgFault::ToPyList(fault_list);
}

PyObject*
DtpgMgr_testvector_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto& tv_list = mgr.testvector_list();
  return PyTestVector::ToPyList(tv_list);
}

PyObject*
DtpgMgr_total_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto val = mgr.total_count();
  return Py_BuildValue("k", val);
}

PyObject*
DtpgMgr_detected_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto val = mgr.detected_count();
  return Py_BuildValue("k", val);
}

PyObject*
DtpgMgr_untestable_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto val = mgr.untestable_count();
  return Py_BuildValue("k", val);
}

PyObject*
DtpgMgr_undetected_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::_get_ref(self);
  auto val = mgr.undetected_count();
  return Py_BuildValue("k", val);
}

PyGetSetDef DtpgMgr_getset[] = {
  {"fault_list", DtpgMgr_fault_list, nullptr,
   PyDoc_STR("fault list"), nullptr},
  {"testvector_list", DtpgMgr_testvector_list, nullptr,
   PyDoc_STR("fault list"), nullptr},
  {"total_count", DtpgMgr_total_count, nullptr,
   PyDoc_STR("total faults count"), nullptr},
  {"detected_count", DtpgMgr_detected_count, nullptr,
   PyDoc_STR("detected faults count"), nullptr},
  {"untestable_count", DtpgMgr_untestable_count, nullptr,
   PyDoc_STR("untestable faults count"), nullptr},
  {"undetected_count", DtpgMgr_undetected_count, nullptr,
   PyDoc_STR("undetected faults count"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
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
  DtpgMgrType.tp_getset = DtpgMgr_getset;
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
PyDtpgMgr::_get_ref(
  PyObject* obj
)
{
  auto mgr_obj = reinterpret_cast<DtpgMgrObject*>(obj);
  return mgr_obj->mVal;
}

// @brief DtpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMgr::_typeobject()
{
  return &DtpgMgrType;
}

END_NAMESPACE_DRUID
