
/// @file PyInputVector.cc
/// @brief PyInputVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#include "InputVector.h"


BEGIN_NAMESPACE_DRUID

extern PyObject* PyObj_from_Val3(Val3);
extern bool Mt19937_from_PyObj(PyObject*, std::mt19937*&);
extern bool Val3_from_PyObj(PyObject*, Val3&);

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct InputVectorObject
{
  PyObject_HEAD
  InputVector* mIv;
};

// Python 用のタイプ定義
PyTypeObject InputVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
InputVector_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  auto self = reinterpret_cast<InputVectorObject*>(type->tp_alloc(type, 0));
  self->mIv = nullptr;
  return reinterpret_cast<PyObject*>(self);
}

// 終了関数
void
InputVector_dealloc(
  InputVectorObject* self
)
{
  delete self->mIv;
  Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
}

// 初期化関数(__init__()相当)
int
InputVector_init(
  InputVectorObject* self,
  PyObject* args
)
{
  SizeType num = 0;
  if ( !PyArg_ParseTuple(args, "i", &num) ) {
    return -1;
  }

  self->mIv = new InputVector{num};
  return 0;
}

// str() 関数
PyObject*
InputVector_str(
  PyObject* self
)
{
  auto tv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto tmp_str = tv_obj->mIv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_len(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  SizeType n = iv_obj->mIv->len();
  return PyLong_FromLong(n);
}

PyObject*
InputVector_val(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "i", &pos) ) {
    return nullptr;
  }
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto val = iv_obj->mIv->val(pos);
  return PyObj_from_Val3(val);
}

PyObject*
InputVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto val = iv_obj->mIv->x_count();
  return PyLong_FromLong(val);
}

PyObject*
InputVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto tmp_str = iv_obj->mIv->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto tmp_str = iv_obj->mIv->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mIv->init();
  Py_RETURN_NONE;
}

PyObject*
InputVector_set_val(
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
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mIv->set_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
InputVector_set_from_random(
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
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mIv->set_from_random(*mt19937);
  Py_RETURN_NONE;
}

PyObject*
InputVector_fix_x_from_random(
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
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mIv->fix_x_from_random(*mt19937);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef InputVector_methods[] = {
  {"len", InputVector_len, METH_NOARGS,
   PyDoc_STR("returns the length")},
  {"val", InputVector_val, METH_VARARGS,
   PyDoc_STR("returns a value of the specified bit")},
  {"x_count", InputVector_x_count, METH_NOARGS,
   PyDoc_STR("returns a number of 'X' bits")},
  {"bin_str", InputVector_bin_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in binary format")},
  {"hex_str", InputVector_hex_str, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"init", InputVector_init_method, METH_NOARGS,
   PyDoc_STR("returns a string representation in hexadecimal format")},
  {"set_val", InputVector_set_val, METH_VARARGS,
   PyDoc_STR("set value of the specified bit")},
  {"set_from_random", InputVector_set_from_random, METH_VARARGS,
   PyDoc_STR("set value randomly")},
  {"fix_x_from_random", InputVector_fix_x_from_random, METH_VARARGS,
   PyDoc_STR("fix 'X' values randomly")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'InputVector' オブジェクトを使用可能にする．
bool
PyInit_InputVector(
  PyObject* m
)
{
  InputVectorType.tp_name = "druid.InputVector";
  InputVectorType.tp_basicsize = sizeof(InputVectorObject);
  InputVectorType.tp_itemsize = 0;
  InputVectorType.tp_dealloc = reinterpret_cast<destructor>(InputVector_dealloc);
  InputVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  InputVectorType.tp_doc = PyDoc_STR("InputVector objects");
  InputVectorType.tp_methods = InputVector_methods;
  InputVectorType.tp_init = reinterpret_cast<initproc>(InputVector_init);
  InputVectorType.tp_new = InputVector_new;
  InputVectorType.tp_str = InputVector_str;

  if ( PyType_Ready(&InputVectorType) < 0 ) {
    return false;
  }
  Py_INCREF(&InputVectorType);
  if ( PyModule_AddObject(m, "InputVector", reinterpret_cast<PyObject*>(&InputVectorType)) < 0 ) {
    Py_DECREF(&InputVectorType);
    return false;
  }
  return true;
}

// @brief PyObject から InputVector を取り出す．
bool
InputVector_from_PyObj(
  PyObject* obj,
  InputVector& iv
)
{
  if ( !Py_IS_TYPE(obj, &InputVectorType) ) {
    PyErr_SetString(PyExc_ValueError, "object is not a InputVector type");
    return false;
  }
  auto iv_obj = reinterpret_cast<InputVectorObject*>(obj);
  iv = *(iv_obj->mIv);
  return true;
}

// @brief InputVector から PyObject を作り出す．
PyObject*
PyObj_from_InputVector(
  const InputVector& iv
)
{
  auto iv_obj = InputVector_new(&InputVectorType, nullptr, nullptr);
  reinterpret_cast<InputVectorObject*>(iv_obj)->mIv = new InputVector{iv};
  return iv_obj;
}

END_NAMESPACE_DRUID
