
/// @file PyTpgFaultStatusMgr.cc
/// @brief Python TpgFaultStatusMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgFaultStatusMgr.h"
#include "PyTpgNetwork.h"
#include "PyTpgFault.h"
#include "PyFaultType.h"
#include "PyFaultStatus.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFaultStatusMgrObject
{
  PyObject_HEAD
  TpgFaultStatusMgr* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFaultStatusMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgFaultStatusMgr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "fault_list",
    nullptr
  };
  PyObject* fault_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O", const_cast<char**>(kwlist),
				    &fault_list_obj) ) {
    return nullptr;
  }
  vector<const TpgFault*> fault_list;
  if ( !PyTpgFault::FromPyList(fault_list_obj, fault_list) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of TpgFault");
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto faultmgr_obj = reinterpret_cast<TpgFaultStatusMgrObject*>(self);
  faultmgr_obj->mVal = new TpgFaultStatusMgr{fault_list};
  return self;
}

// 終了関数
void
TpgFaultStatusMgr_dealloc(
  PyObject* self
)
{
  auto tpgfaultmgr_obj = reinterpret_cast<TpgFaultStatusMgrObject*>(self);
  delete tpgfaultmgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
TpgFaultStatusMgr_get_status(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!",
			 PyTpgFault::_typeobject(), &fault_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::Get(fault_obj);
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto ans = fmgr.get_status(fault);
  return PyFaultStatus::ToPyObject(ans);
}

PyObject*
TpgFaultStatusMgr_set_status(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  PyObject* status_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PyTpgFault::_typeobject(), &fault_obj,
			 PyFaultStatus::_typeobject(), &status_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::Get(fault_obj);
  auto status = PyFaultStatus::Get(status_obj);
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  fmgr.set_status(fault, status);
  Py_RETURN_NONE;
}


// メソッド定義
PyMethodDef TpgFaultStatusMgr_methods[] = {
  {"get_status", TpgFaultStatusMgr_get_status, METH_VARARGS,
   PyDoc_STR("get fault status")},
  {"set_status", TpgFaultStatusMgr_set_status, METH_VARARGS,
   PyDoc_STR("set fault status")},
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
TpgFaultStatusMgr_fault_list(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto& ans = fmgr.fault_list();
  return PyTpgFault::ToPyList(ans);
}

PyObject*
TpgFaultStatusMgr_total_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto ans = fmgr.total_count();
  return Py_BuildValue("i", ans);
}

PyObject*
TpgFaultStatusMgr_detected_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto ans = fmgr.detected_count();
  return Py_BuildValue("i", ans);
}

PyObject*
TpgFaultStatusMgr_untestable_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto ans = fmgr.untestable_count();
  return Py_BuildValue("i", ans);
}

PyObject*
TpgFaultStatusMgr_remain_count(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& fmgr = PyTpgFaultStatusMgr::Get(self);
  auto ans = fmgr.remain_count();
  return Py_BuildValue("i", ans);
}

PyGetSetDef TpgFaultStatusMgr_getset[] = {
  {"total_count", TpgFaultStatusMgr_total_count, nullptr,
   PyDoc_STR("total count"), nullptr},
  {"detected_count", TpgFaultStatusMgr_detected_count, nullptr,
   PyDoc_STR("detected count"), nullptr},
  {"untestable_count", TpgFaultStatusMgr_untestable_count, nullptr,
   PyDoc_STR("untestable count"), nullptr},
  {"remain_count", TpgFaultStatusMgr_remain_count, nullptr,
   PyDoc_STR("total count"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'TpgFaultStatusMgr' オブジェクトを使用可能にする．
bool
PyTpgFaultStatusMgr::init(
  PyObject* m
)
{
  TpgFaultStatusMgrType.tp_name = "TpgFaultStatusMgr";
  TpgFaultStatusMgrType.tp_basicsize = sizeof(TpgFaultStatusMgrObject);
  TpgFaultStatusMgrType.tp_itemsize = 0;
  TpgFaultStatusMgrType.tp_dealloc = TpgFaultStatusMgr_dealloc;
  TpgFaultStatusMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultStatusMgrType.tp_doc = PyDoc_STR("TpgFaultStatusMgr object");
  TpgFaultStatusMgrType.tp_methods = TpgFaultStatusMgr_methods;
  TpgFaultStatusMgrType.tp_getset = TpgFaultStatusMgr_getset;
  TpgFaultStatusMgrType.tp_new = TpgFaultStatusMgr_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgFaultStatusMgr", &TpgFaultStatusMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が TpgFaultStatusMgr タイプか調べる．
bool
PyTpgFaultStatusMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgFaultStatusMgr を表す PyObject から TpgFaultStatusMgr を取り出す．
TpgFaultStatusMgr&
PyTpgFaultStatusMgr::Get(
  PyObject* obj
)
{
  auto tpgfaultmgr_obj = reinterpret_cast<TpgFaultStatusMgrObject*>(obj);
  return *tpgfaultmgr_obj->mVal;
}

// @brief TpgFaultStatusMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFaultStatusMgr::_typeobject()
{
  return &TpgFaultStatusMgrType;
}

END_NAMESPACE_DRUID
