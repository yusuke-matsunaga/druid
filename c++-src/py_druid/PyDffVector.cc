
/// @file PyDffVector.cc
/// @brief Python DffVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PyDffVector.h"
#include "ym/PyMt19937.h"
#include "PyVal3.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DffVectorObject
{
  PyObject_HEAD
  DffVector* mPtr;
};

// Python 用のタイプ定義
PyTypeObject DffVectorType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DffVector_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(self);
  dffvector_obj->mPtr = nullptr;
  return self;
}

// 終了関数
void
DffVector_dealloc(
  PyObject* self
)
{
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(self);
  delete dffvector_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
DffVector_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  SizeType num = 0;
  if ( !PyArg_ParseTuple(args, "i", &num) ) {
    return -1;
  }

  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(self);
  dffvector_obj->mPtr = new DffVector{num};
  return 0;
}

// str() 関数
PyObject*
DffVector_str(
  PyObject* self
)
{
  auto tv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = tv_obj->mPtr->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_len(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  SizeType n = dv_obj->mPtr->len();
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
  auto val = dv_obj->mPtr->val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
DffVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto val = dv_obj->mPtr->x_count();
  return PyLong_FromLong(val);
}

PyObject*
DffVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = dv_obj->mPtr->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  auto tmp_str = dv_obj->mPtr->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
DffVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mPtr->init();
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
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mPtr->set_val(pos, val);
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
  if ( !PyMt19937::FromPyObject(obj, mt19937) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mPtr->set_from_random(*mt19937);
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
  if ( !PyMt19937::FromPyObject(obj, mt19937) ) {
    return nullptr;
  }
  auto dv_obj = reinterpret_cast<DffVectorObject*>(self);
  dv_obj->mPtr->fix_x_from_random(*mt19937);
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
PyDffVector::init(
  PyObject* m
)
{
  DffVectorType.tp_name = "DffVector";
  DffVectorType.tp_basicsize = sizeof(DffVectorObject);
  DffVectorType.tp_itemsize = 0;
  DffVectorType.tp_dealloc = DffVector_dealloc;
  DffVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  DffVectorType.tp_doc = PyDoc_STR("DffVector objects");
  DffVectorType.tp_init = DffVector_init;
  DffVectorType.tp_new = DffVector_new;
  if ( PyType_Ready(&DffVectorType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&DffVectorType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "DffVector", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から DffVector を取り出す．
bool
PyDffVector::FromPyObject(
  PyObject* obj,
  DffVector& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a DffVector type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief DffVector を PyObject に変換する．
PyObject*
PyDffVector::ToPyObject(
  const DffVector& val
)
{
  auto obj = DffVector_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が DffVector タイプか調べる．
bool
PyDffVector::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DffVector を表す PyObject から DffVector を取り出す．
const DffVector&
PyDffVector::_get(
  PyObject* obj
)
{
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(obj);
  return *dffvector_obj->mPtr;
}

// @brief DffVector を表す PyObject に値を設定する．
void
PyDffVector::_put(
  PyObject* obj,
  const DffVector& val
)
{
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(obj);
  *dffvector_obj->mPtr = val;
}

// @brief DffVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDffVector::_typeobject()
{
  return &DffVectorType;
}

END_NAMESPACE_DRUID
