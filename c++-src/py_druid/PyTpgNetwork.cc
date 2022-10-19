
/// @file PyTpgNetwork.cc
/// @brief PyTpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

extern PyObject* PyObj_from_TpgFault(const TpgFault*);

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgNetworkObject
{
  PyObject_HEAD
  TpgNetwork* mNetwork;
};

// 生成関数
PyObject*
TpgNetwork_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = reinterpret_cast<TpgNetworkObject*>(type->tp_alloc(type, 0));
  self->mNetwork = new TpgNetwork;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
TpgNetwork_dealloc(
  TpgNetworkObject* self
)
{
  delete self->mNetwork;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
TpgNetwork_init(
  TpgNetworkObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "blif_file",
    "iscas89_file",
    nullptr
  };
  const char* blif_file = nullptr;
  const char* iscas89_file = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds,
				    "|$ss", const_cast<char**>(kwlist),
				    &blif_file,
				    &iscas89_file) ) {
    return -1;
  }
  if ( blif_file != nullptr ) {
    if ( iscas89_file != nullptr ) {
      PyErr_SetString(PyExc_ValueError,
		      "'blif_file' and 'iscas89_file' are mutually exclusive");
      return -1;
    }
    // blif ファイルを読み込む．
    if ( !self->mNetwork->read_blif(blif_file) ) {
      PyErr_SetString(PyExc_ValueError, "read failed");
      return -1;
    }
  }
  else if ( iscas89_file != nullptr ) {
    // iscas89 ファイルを読み込む．
    if ( !self->mNetwork->read_iscas89(iscas89_file) ) {
      PyErr_SetString(PyExc_ValueError, "read failed");
      return -1;
    }
  }
  else {
    // 空のまま
  }
  return 0;
}

PyObject*
TpgNetwork_rep_fault_list(
  TpgNetworkObject* self,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto& fault_list = self->mNetwork->rep_fault_list();
  SizeType n = fault_list.size();
  auto ans_obj = PyList_New(n);
  Py_IncRef(ans_obj);
  SizeType index = 0;
  for ( auto fault: fault_list ) {
    auto fault_obj = PyObj_from_TpgFault(fault);
    PyList_SET_ITEM(ans_obj, index, fault_obj);
    ++ index;
  }
  return ans_obj;
}

// メソッド定義
PyMethodDef TpgNetwork_methods[] = {
  {"rep_fault_list", reinterpret_cast<PyCFunction>(TpgNetwork_rep_fault_list),
   METH_NOARGS, PyDoc_STR("generate a list for all representative faults")},
  {nullptr, nullptr, 0, nullptr}
};

// Python 用のタイプ定義
PyTypeObject TpgNetworkType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

END_NONAMESPACE


// @brief 'Val3' オブジェクトを使用可能にする．
bool
PyInit_TpgNetwork(
  PyObject* m
)
{
  TpgNetworkType.tp_name = "druid.TpgNetwork";
  TpgNetworkType.tp_basicsize = sizeof(TpgNetworkObject);
  TpgNetworkType.tp_itemsize = 0;
  TpgNetworkType.tp_dealloc = reinterpret_cast<destructor>(TpgNetwork_dealloc);
  TpgNetworkType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgNetworkType.tp_doc = PyDoc_STR("TpgNetwork objects");
  TpgNetworkType.tp_methods = TpgNetwork_methods;
  TpgNetworkType.tp_init = reinterpret_cast<initproc>(TpgNetwork_init);
  TpgNetworkType.tp_new = TpgNetwork_new;
  if ( PyType_Ready(&TpgNetworkType) < 0 ) {
    return false;
  }
  Py_INCREF(&TpgNetworkType);
  if ( PyModule_AddObject(m, "TpgNetwork",
			  reinterpret_cast<PyObject*>(&TpgNetworkType)) < 0 ) {
    Py_DECREF(&TpgNetworkType);
    return false;
  }
  return true;
}

// @brief PyObject から TpgNetwork を取り出す．
bool
TpgNetwork_from_PyObj(
  PyObject* obj,
  TpgNetwork*& network
)
{
  if ( !Py_IS_TYPE(obj, &TpgNetworkType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a TpgNetwork type");
    return false;
  }
  auto network_obj = reinterpret_cast<TpgNetworkObject*>(obj);
  network = network_obj->mNetwork;
  return true;
}

END_NAMESPACE_DRUID
