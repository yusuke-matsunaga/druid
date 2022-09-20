
/// @file PyDffVector.cc
/// @brief PyDffVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "DffVector.h"


BEGIN_NAMESPACE_DRUID

extern PyObject* PyObj_from_Val3(Val3);
extern bool Mt19937_from_PyObj(PyObject*, std::mt19937*&);
extern bool Val3_from_PyObj(PyObject*, Val3&);

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DffVectorObject
{
  PyObject_HEAD
  DffVector* mDv;
};

// Python 用のタイプ定義
PyTypeObject DffVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DffVector_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  auto self = reinterpret_cast<DffVectorObject*>(type->tp_alloc(type, 0));
  self->mDv = nullptr;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
DffVector_dealloc(
  DffVectorObject* self
)
{
  delete self->mDv;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
DffVector_init(
  DffVectorObject* self,
  PyObject* args
)
{
  SizeType num = 0;
  if ( !PyArg_ParseTuple(args, "i", &num) ) {
    return -1;
  }

  self->mDv = new DffVector{num};
  return 0;
}

// str() 関数
PyObject*
DffVector_str(
  PyObject* self
)
{
  auto tv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = tv_obj->mDv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_len(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  SizeType n = dv_obj->mDv->len();
  return PyLong_FromLong(n);
}

PyObject*
DffVector_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto val = dv_obj->mDv->val(pos);
  return PyObj_from_Val3(val);
}

PyObject*
DffVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto val = dv_obj->mDv->x_count();
  return PyLong_FromLong(val);
}

PyObject*
DffVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = dv_obj->mDv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = dv_obj->mDv->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mDv->init();
  Py_RETURN_NONE;
}

PyObject*
DffVector_set_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "iO", &pos, &obj) ) {
    return nullptr;
  }
  Val3 val;
  if ( !Val3_from_PyObj(obj, val) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mDv->set_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
DffVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  std::mt19937* mt19937;
  if ( !Mt19937_from_PyObj(obj, mt19937) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mDv->set_from_random(*mt19937);
  Py_RETURN_NONE;
}

PyObject*
DffVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &obj) ) {
    return nullptr;
  }

  std::mt19937* mt19937;
  if ( !Mt19937_from_PyObj(obj, mt19937) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mDv->fix_x_from_random(*mt19937);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DffVector_methods[] = {
  {"len", DffVector_len, METH_NOARGS,
   PyDoc_STR("returns the length")},
  {"val", DffVector_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified bit")},
  {"x_count", DffVector_x_count, METH_NOARGS,
   PyDoc_STR("returns a number of 'X' bits")},
  {"bin_str", DffVector_bin_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in binary format")},
  {"hex_str", DffVector_hex_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"init", DffVector_init_method, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"set_val", DffVector_set_val, METH_VARARGS,
   PyDoc_STR("set value of the specified bit")},
  {"set_from_random", DffVector_set_from_random, METH_VARARGS,
   PyDoc_STR("set value randomly")},
  {"fix_x_from_random", DffVector_fix_x_from_random, METH_VARARGS,
   PyDoc_STR("fix 'X' values randomly")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'DffVector' オブジェクトを使用可能にする．
bool
PyInit_DffVector(
  PyObject* m
)
{
  DffVectorType.tp_name = "druid.DffVector";
  DffVectorType.tp_basicsize = sizeof(DffVectorObject);
  DffVectorType.tp_itemsize = 0;
  DffVectorType.tp_dealloc = reinterpret_cast<destructor>(DffVector_dealloc);
  DffVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  DffVectorType.tp_doc = PyDoc_STR("DffVector objects");
  DffVectorType.tp_methods = DffVector_methods;
  DffVectorType.tp_init = reinterpret_cast<initproc>(DffVector_init);
  DffVectorType.tp_new = DffVector_new;
  DffVectorType.tp_str = DffVector_str;

  if ( PyType_Ready(&DffVectorType) < 0 ) {
    return false;
  }
  Py_INCREF(&DffVectorType);
  if ( PyModule_AddObject(m, "DffVector", reinterpret_cast<PyObject*>(&DffVectorType)) < 0 ) {
    Py_DECREF(&DffVectorType);
    return false;
  }
  return true;
}

// @brief PyObject から DffVector を取り出す．
bool
DffVector_from_PyObj(
  PyObject* obj,
  DffVector& dv
)
{
  if ( !Py_IS_TYPE(obj, &DffVectorType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a DffVector type");
    return false;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(obj);
  dv = *(dv_obj->mDv);
  return true;
}

// @brief DffVector から PyObject を作り出す．
PyObject*
PyObj_from_DffVector(
  const DffVector& dv
)
{
  auto dv_obj = DffVector_new(&DffVectorType, nullptr, nullptr);
  reinterpret_cast<DffVectorObject*>(dv_obj)->mDv = new DffVector{dv};
  return dv_obj;
}

END_NAMESPACE_DRUID
