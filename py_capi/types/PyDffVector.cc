
/// @file PyDffVector.cc
/// @brief Python DffVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDffVector.h"
#include "pym/PyVal3.h"
#include "pym/PyMt19937.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DffVectorObject
{
  PyObject_HEAD
  DffVector mVal;
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
  static const char* kwlist[] = {
    "num",
    nullptr
  };
  SizeType num = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i",
				    const_cast<char**>(kwlist),
				    &num) ) {
    return nullptr;
  }
  auto obj = type->tp_alloc(type, 0);
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(obj);
  new (&dffvector_obj->mVal) DffVector(num);
  return obj;
}

// 終了関数
void
DffVector_dealloc(
  PyObject* self
)
{
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(self);
  dffvector_obj->mVal.~DffVector();
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
DffVector_str(
  PyObject* self
)
{
  auto& dv = PyDffVector::_get_ref(self);
  return PyString::ToPyObject(dv.bin_str());
}

PyObject*
DffVector_len(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& dv = PyDffVector::_get_ref(self);
  return PyLong_FromLong(dv.len());
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
  auto& dv = PyDffVector::_get_ref(self);
  auto val = dv.val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
DffVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& dv = PyDffVector::_get_ref(self);
  auto val = dv.x_count();
  return PyLong_FromLong(val);
}

PyObject*
DffVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& dv = PyDffVector::_get_ref(self);
  return PyString::ToPyObject(dv.bin_str());
}

PyObject*
DffVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& dv = PyDffVector::_get_ref(self);
  return PyString::ToPyObject(dv.hex_str());
}

PyObject*
DffVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& dv = PyDffVector::_get_ref(self);
  dv.init();
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
  auto& dv = PyDffVector::_get_ref(self);
  dv.set_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
DffVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto& dv = PyDffVector::_get_ref(self);
  dv.set_from_random(mt19937);
  Py_RETURN_NONE;
}

PyObject*
DffVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::_get_ref(obj);
  auto& dv = PyDffVector::_get_ref(self);
  dv.fix_x_from_random(mt19937);
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
  DffVectorType.tp_doc = PyDoc_STR("DffVector object");
  DffVectorType.tp_new = DffVector_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DffVector", &DffVectorType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DffVector を PyObject に変換する．
PyObject*
PyDffVector::Conv::operator()(
  const DffVector& val
)
{
  auto obj = DffVectorType.tp_alloc(&DffVectorType, 0);
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(obj);
  new (&dffvector_obj->mVal) DffVector(val);
  return obj;
}

// @brief PyObject* から DffVector を取り出す．
bool
PyDffVector::Deconv::operator()(
  PyObject* obj,
  DffVector& val
)
{
  if ( PyDffVector::Check(obj) ) {
    val = PyDffVector::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DffVector タイプか調べる．
bool
PyDffVector::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DffVector を表す PyObject から DffVector を取り出す．
DffVector&
PyDffVector::_get_ref(
  PyObject* obj
)
{
  auto dffvector_obj = reinterpret_cast<DffVectorObject*>(obj);
  return dffvector_obj->mVal;
}

// @brief DffVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDffVector::_typeobject()
{
  return &DffVectorType;
}

END_NAMESPACE_DRUID
