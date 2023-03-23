
/// @file PyTpgFaultMgr.cc
/// @brief Python TpgFaultMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyTpgFaultMgr.h"
#include "PyTpgNetwork.h"
#include "PyTpgFault.h"
#include "PyFaultType.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFaultMgrObject
{
  PyObject_HEAD
  TpgFaultMgr* mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFaultMgrType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
TpgFaultMgr_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    nullptr
  };
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "", const_cast<char**>(kwlist)) ) {
    return nullptr;
  }
  auto self = type->tp_alloc(type, 0);
  auto tpgfaultmgr_obj = reinterpret_cast<TpgFaultMgrObject*>(self);
  tpgfaultmgr_obj->mVal = new TpgFaultMgr;
  return self;
}

// 終了関数
void
TpgFaultMgr_dealloc(
  PyObject* self
)
{
  auto tpgfaultmgr_obj = reinterpret_cast<TpgFaultMgrObject*>(self);
  delete tpgfaultmgr_obj->mVal;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
TpgFaultMgr_gen_fault_list(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "network",
    "fault_type",
    nullptr
  };
  PyObject* network_obj = nullptr;
  PyObject* fault_type_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
				    const_cast<char**>(kwlist),
				    PyTpgNetwork::_typeobject(), &network_obj,
				    PyFaultType::_typeobject(), &fault_type_obj) ) {
    return nullptr;
  }
  auto& network = PyTpgNetwork::Get(network_obj);
  auto fault_type = PyFaultType::Get(fault_type_obj);
  auto& fmgr = PyTpgFaultMgr::Get(self);
  fmgr.gen_fault_list(network, fault_type);
  Py_RETURN_NONE;
}

PyObject*
TpgFaultMgr_fault_type(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_type = fmgr.fault_type();
  return PyFaultType::ToPyObject(fault_type);
}

BEGIN_NONAMESPACE

PyObject*
fault_list_to_pyobj(
  const TpgFaultList& fault_list
)
{
  SizeType n = fault_list.size();
  auto ans_obj = PyList_New(n);
  Py_IncRef(ans_obj);
  SizeType index = 0;
  for ( auto fault: fault_list ) {
    auto fault_obj = PyTpgFault::ToPyObject(fault);
    PyList_SetItem(ans_obj, index, fault_obj);
    ++ index;
  }
  return ans_obj;
}

END_NONAMESPACE

PyObject*
TpgFaultMgr_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.fault_list();
  return fault_list_to_pyobj(fault_list);
}

PyObject*
TpgFaultMgr_node_fault_list(
  PyObject* self,
  PyObject* args
)
{
  SizeType id{0};
  if ( !PyArg_ParseTuple(args, "i", &id) ) {
    return nullptr;
  }
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.node_fault_list(id);
  return fault_list_to_pyobj(fault_list);
}

PyObject*
TpgFaultMgr_ffr_fault_list(
  PyObject* self,
  PyObject* args
)
{
  SizeType id{0};
  if ( !PyArg_ParseTuple(args, "i", &id) ) {
    return nullptr;
  }
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.ffr_fault_list(id);
  return fault_list_to_pyobj(fault_list);
}

PyObject*
TpgFaultMgr_mffc_fault_list(
  PyObject* self,
  PyObject* args
)
{
  SizeType id = 0;
  if ( !PyArg_ParseTuple(args, "i", &id) ) {
    return nullptr;
  }
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.mffc_fault_list(id);
  return fault_list_to_pyobj(fault_list);
}

PyObject*
TpgFaultMgr_set(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  const char* fault_status_str = nullptr;
  if ( !PyArg_ParseTuple(args, "O!s",
			 PyTpgFault::_typeobject(), &fault_obj,
			 &fault_status_str) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::Get(fault_obj);
  auto fault_status = FaultStatus::Undetected;
  if ( strcmp(fault_status_str, "detected") == 0 ) {
    fault_status = FaultStatus::Detected;
  }
  else if ( strcmp(fault_status_str, "untestable") == 0 ) {
    fault_status = FaultStatus::Untestable;
  }
  else if ( strcmp(fault_status_str, "undetected") == 0 ) {
    fault_status = FaultStatus::Undetected;
  }
  else {
    PyErr_SetString(PyExc_ValueError, "illegal string for FaultStatus");
    return nullptr;
  }
  auto& fmgr = PyTpgFaultMgr::Get(self);
  fmgr.set_status(fault, fault_status);
  Py_RETURN_NONE;
}

PyObject*
TpgFaultMgr_get(
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
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_status = fmgr.get_status(fault);
  auto fault_status_str = str(fault_status);
  return Py_BuildValue("s", fault_status_str);
}

// メソッド定義
PyMethodDef TpgFaultMgr_methods[] = {
  {"gen_fault_list", reinterpret_cast<PyCFunction>(TpgFaultMgr_gen_fault_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate faults list")},
  {"set_status", TpgFaultMgr_set, METH_VARARGS,
   PyDoc_STR("set fault status")},
  {"get_status", TpgFaultMgr_get, METH_VARARGS,
   PyDoc_STR("get fault status")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'TpgFaultMgr' オブジェクトを使用可能にする．
bool
PyTpgFaultMgr::init(
  PyObject* m
)
{
  TpgFaultMgrType.tp_name = "TpgFaultMgr";
  TpgFaultMgrType.tp_basicsize = sizeof(TpgFaultMgrObject);
  TpgFaultMgrType.tp_itemsize = 0;
  TpgFaultMgrType.tp_dealloc = TpgFaultMgr_dealloc;
  TpgFaultMgrType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultMgrType.tp_doc = PyDoc_STR("TpgFaultMgr object");
  TpgFaultMgrType.tp_methods = TpgFaultMgr_methods;
  TpgFaultMgrType.tp_new = TpgFaultMgr_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "TpgFaultMgr", &TpgFaultMgrType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が TpgFaultMgr タイプか調べる．
bool
PyTpgFaultMgr::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief TpgFaultMgr を表す PyObject から TpgFaultMgr を取り出す．
TpgFaultMgr&
PyTpgFaultMgr::Get(
  PyObject* obj
)
{
  auto tpgfaultmgr_obj = reinterpret_cast<TpgFaultMgrObject*>(obj);
  return *tpgfaultmgr_obj->mVal;
}

// @brief TpgFaultMgr を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFaultMgr::_typeobject()
{
  return &TpgFaultMgrType;
}

END_NAMESPACE_DRUID
