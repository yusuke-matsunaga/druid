
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
#include "PyFaultStatus.h"
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

PyObject*
TpgFaultMgr_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.fault_list();
  return PyTpgFault::ToPyList(fault_list);
}

PyObject*
TpgFaultMgr_rep_fault_list(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& fmgr = PyTpgFaultMgr::Get(self);
  auto fault_list = fmgr.rep_fault_list();
  return PyTpgFault::ToPyList(fault_list);
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
  return PyTpgFault::ToPyList(fault_list);
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
  return PyTpgFault::ToPyList(fault_list);
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
  return PyTpgFault::ToPyList(fault_list);
}

PyObject*
TpgFaultMgr_set(
  PyObject* self,
  PyObject* args
)
{
  PyObject* fault_obj = nullptr;
  PyObject* fault_status_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PyTpgFault::_typeobject(), &fault_obj,
			 PyFaultStatus::_typeobject(), &fault_status_obj) ) {
    return nullptr;
  }
  auto fault = PyTpgFault::Get(fault_obj);
  auto fault_status = PyFaultStatus::Get(fault_status_obj);
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
  return PyFaultStatus::ToPyObject(fault_status);
}

// メソッド定義
PyMethodDef TpgFaultMgr_methods[] = {
  {"gen_fault_list", reinterpret_cast<PyCFunction>(TpgFaultMgr_gen_fault_list),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("generate faults list")},
  {"fault_type", TpgFaultMgr_fault_type, METH_NOARGS,
   PyDoc_STR("FaultType")},
  {"fault_list", TpgFaultMgr_fault_list, METH_NOARGS,
   PyDoc_STR("list of all faults")},
  {"rep_fault_list", TpgFaultMgr_rep_fault_list, METH_NOARGS,
   PyDoc_STR("list of all representative faults")},
  {"node_fault_list", TpgFaultMgr_node_fault_list, METH_VARARGS,
   PyDoc_STR("list of all faults of the specified node")},
  {"ffr_fault_list", TpgFaultMgr_ffr_fault_list, METH_VARARGS,
   PyDoc_STR("list of all faults of the specified FFR")},
  {"mffc_fault_list", TpgFaultMgr_mffc_fault_list, METH_VARARGS,
   PyDoc_STR("list of all faults of the specified MFFC")},
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
