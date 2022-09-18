
/// @file PyMt19937.cc
/// @brief PyMt19937 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include <random>


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct Mt19937Object
{
  PyObject_HEAD
  std::mt19937* mMt19937;
};

// 生成関数
PyObject*
Mt19937_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  auto self = reinterpret_cast<Mt19937Object*>(type->tp_alloc(type, 0));
  self->mMt19937 = new std::mt19937;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
Mt19937_dealloc(
  Mt19937Object* self
)
{
  delete self->mMt19937;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
Mt19937_init(
  Mt19937Object* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  int val = 0;
  if ( !PyArg_ParseTuple(args, "|i", &val) ) {
    return -1;
  }
  if ( val != 0 ) {
    self->mMt19937->seed(val);
  }
  return 0;
}

// eval() 関数
PyObject*
Mt19937_eval(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto mt_obj = reinterpret_cast<Mt19937Object*>(self);
  auto val = mt_obj->mMt19937->operator()();
  return PyLong_FromLong(val);
}

// メソッド定義
PyMethodDef Mt19937_methods[] = {
  {"eval", Mt19937_eval, METH_NOARGS,
   PyDoc_STR("generate a random number")},
  {nullptr, nullptr, 0, nullptr}
};

// Python 用のタイプ定義
PyTypeObject Mt19937Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

END_NONAMESPACE


// @brief 'Mt19937' オブジェクトを使用可能にする．
bool
PyInit_Mt19937(
  PyObject* m
)
{
  Mt19937Type.tp_name = "druid.Mt19937";
  Mt19937Type.tp_basicsize = sizeof(Mt19937Object);
  Mt19937Type.tp_itemsize = 0;
  Mt19937Type.tp_dealloc = reinterpret_cast<destructor>(Mt19937_dealloc);
  Mt19937Type.tp_flags = Py_TPFLAGS_DEFAULT;
  Mt19937Type.tp_doc = PyDoc_STR("Mt19937 objects");
  Mt19937Type.tp_methods = Mt19937_methods;
  Mt19937Type.tp_init = reinterpret_cast<initproc>(Mt19937_init);
  Mt19937Type.tp_new = Mt19937_new;
  if ( PyType_Ready(&Mt19937Type) < 0 ) {
    return false;
  }

  Py_INCREF(&Mt19937Type);
  if ( PyModule_AddObject(m, "Mt19937", reinterpret_cast<PyObject*>(&Mt19937Type)) < 0 ) {
    Py_DECREF(&Mt19937Type);
    return false;
  }

  return true;
}

bool
Mt19937_from_PyObj(
  PyObject* obj,
  std::mt19937*& mt19937
)
{
  if ( !Py_IS_TYPE(obj, &Mt19937Type) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a Mt19937 type");
    return false;
  }
  auto mt_obj = reinterpret_cast<Mt19937Object*>(obj);
  mt19937 = mt_obj->mMt19937;
  return true;
}

END_NAMESPACE_DRUID
