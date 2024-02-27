
/// @file PyDtpgMgr.cc
/// @brief Python DtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyDtpgMgr.h"
#include "pym/PyModule.h"
#include "PyFsim.h"
#include "PyTpgNetwork.h"
#include "PyTpgFaultMgr.h"
#include "PyTestVector.h"
#include "pym/PyJsonValue.h"
#include "ym/SatInitParam.h"


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
    "fault_mgr",
    "option",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_mgr_obj = nullptr;
  PyObject* option_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!|O",
				    const_cast<char**>(kw_list),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyTpgFaultMgr::_typeobject(), &fault_mgr_obj,
				    &option_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  auto& fault_mgr = PyTpgFaultMgr::Get(fault_mgr_obj);
  JsonValue option;
  if ( !PyJsonValue::ConvToJsonValue(option_obj, option) ) {
    PyErr_SetString(PyExc_ValueError, "illegal value for option");
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  bool multi = false;
  tpgmgr_obj->mVal = new DtpgMgr{network, fault_mgr, option, multi};
  return self;
}

// 終了関数
void
DtpgMgr_dealloc(
  PyObject* self
)
{
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(self);
  delete tpgmgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DtpgMgr_run(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  mgr.run();
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_dop(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "name",
    nullptr
  };

  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
				    const_cast<char**>(kwlist),
				    &obj) ) {
    return nullptr;
  }
  JsonValue js_obj;
  if ( !PyJsonValue::ConvToJsonValue(obj, js_obj) ) {
    PyErr_SetString(PyExc_ValueError, "illegal value");
    return nullptr;
  }

  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_dop(js_obj);
  Py_RETURN_NONE;
}

PyObject*
DtpgMgr_add_uop(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "name",
    nullptr
  };
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
				    const_cast<char**>(kwlist),
				    &obj) ) {
    return nullptr;
  }
  JsonValue js_obj;
  if ( !PyJsonValue::ConvToJsonValue(obj, js_obj) ) {
    PyErr_SetString(PyExc_ValueError, "illegal value");
    return nullptr;
  }

  auto& mgr = PyDtpgMgr::Get(self);
  mgr.add_uop(js_obj);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DtpgMgr_methods[] = {
  {"run", DtpgMgr_run, METH_NOARGS,
   PyDoc_STR("run")},
  {"add_dop", reinterpret_cast<PyCFunction>(DtpgMgr_add_dop),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add DetectOp")},
  {"add_uop", reinterpret_cast<PyCFunction>(DtpgMgr_add_uop),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add UntestOp")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
DtpgMgr_detect_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.detect_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_untest_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.untest_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_abort_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto val = mgr.abort_count();
  return PyLong_FromLong(val);
}

PyObject*
DtpgMgr_tv_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto& tv_list = mgr.tv_list();
  return PyTestVector::ToPyList(tv_list);
}

PyObject*
DtpgMgr_stats(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& mgr = PyDtpgMgr::Get(self);
  auto& stats = mgr.dtpg_stats();
  // 未完
  Py_RETURN_NONE;
}

// get/set 関数定義
PyGetSetDef DtpgMgr_getset[] = {
  {"detect_count", DtpgMgr_detect_count, nullptr,
   PyDoc_STR("# of detected faults")},
  {"untest_count", DtpgMgr_untest_count, nullptr,
   PyDoc_STR("# of untestable faults")},
  {"abort_count", DtpgMgr_abort_count, nullptr,
   PyDoc_STR("# of aborted faults")},
  {"tv_list", DtpgMgr_tv_list, nullptr,
   PyDoc_STR("list of TestVector")},
  {"dtpg_stats", DtpgMgr_stats, nullptr,
   PyDoc_STR("statistics of DTPG operation")},
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
  DtpgMgrType.tp_getset = DtpgMgr_getset;

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
  auto tpgmgr_obj = reinterpret_cast<DtpgMgrObject*>(obj);
  return *tpgmgr_obj->mVal;
}

// @brief DtpgMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDtpgMgr::_typeobject()
{
  return &DtpgMgrType;
}

END_NAMESPACE_DRUID
