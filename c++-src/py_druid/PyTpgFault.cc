
/// @file PyTpgFalt.cc
/// @brief TpgFault の Python 拡張
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct TpgFaultObject
{
  PyObject_HEAD
  const TpgFault* mFault;
};

// 生成関数
PyObject*
TpgFault_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = reinterpret_cast<TpgFaultObject*>(type->tp_alloc(type, 0));
  self->mFault = nullptr;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
TpgFault_dealloc(
  TpgFaultObject* self
)
{
  // なにもしない．
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
TpgFault_init(
  TpgFaultObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  // なにもしない．
  return 0;
}

// str() 関数
PyObject*
TpgFault_str(
  PyObject* self
)
{
  auto tmp_str = reinterpret_cast<TpgFaultObject*>(self)->mFault->str();
  return Py_BuildValue("s", tmp_str.c_str());
}

// メソッド定義
PyMethodDef TpgFault_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// Python 用のタイプ定義
PyTypeObject TpgFaultType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

END_NONAMESPACE


// @brief 'TpgFault' オブジェクトを使用可能にする．
bool
PyInit_TpgFault(
  PyObject* m
)
{
  TpgFaultType.tp_name = "druid.TpgFault";
  TpgFaultType.tp_basicsize = sizeof(TpgFaultObject);
  TpgFaultType.tp_itemsize = 0;
  TpgFaultType.tp_dealloc = reinterpret_cast<destructor>(TpgFault_dealloc);
  TpgFaultType.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFaultType.tp_doc = PyDoc_STR("TpgFault objects");
  TpgFaultType.tp_methods = TpgFault_methods;
  TpgFaultType.tp_init = reinterpret_cast<initproc>(TpgFault_init);
  TpgFaultType.tp_new = TpgFault_new;
  TpgFaultType.tp_str = TpgFault_str;
  if ( PyType_Ready(&TpgFaultType) < 0 ) {
    return false;
  }
  Py_INCREF(&TpgFaultType);
  if ( PyModule_AddObject(m, "TpgFault", reinterpret_cast<PyObject*>(&TpgFaultType)) < 0 ) {
    Py_DECREF(&TpgFaultType);
    return false;
  }
  return true;
}

// @brief PyObject から TpgFault を取り出す．
bool
TpgFault_from_PyObj(
  PyObject* obj,
  const TpgFault*& fault
)
{
  if ( !Py_IS_TYPE(obj, &TpgFaultType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a TpgFault type");
    return false;
  }
  auto fault_obj = reinterpret_cast<TpgFaultObject*>(obj);
  fault = fault_obj->mFault;
  return true;
}

// @brief TpgFault から PyObject を作り出す．
PyObject*
PyObj_from_TpgFault(
  const TpgFault* fault
)
{
  auto fault_obj = TpgFault_new(&TpgFaultType, nullptr, nullptr);
  reinterpret_cast<TpgFaultObject*>(fault_obj)->mFault = fault;
  return fault_obj;
}

END_NAMESPACE_DRUID
