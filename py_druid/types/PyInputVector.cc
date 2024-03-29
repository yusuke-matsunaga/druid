
/// @file PyInputVector.cc
/// @brief Python InputVector の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "PyInputVector.h"
#include "PyVal3.h"
#include "pym/PyMt19937.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct InputVectorObject
{
  PyObject_HEAD
  InputVector* mPtr;
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
  auto inputvector_obj = reinterpret_cast<InputVectorObject*>(obj);
  inputvector_obj->mPtr = new InputVector{num};
  return obj;
}

// 終了関数
void
InputVector_dealloc(
  PyObject* self
)
{
  auto inputvector_obj = reinterpret_cast<InputVectorObject*>(self);
  delete inputvector_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

// str() 関数
PyObject*
InputVector_str(
  PyObject* self
)
{
  auto& val = PyInputVector::Get(self);
  // val から 文字列を作る．
  auto tmp_str = val.bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_len(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  SizeType n = iv_obj->mPtr->len();
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
  auto val = iv_obj->mPtr->val(pos);
  return PyVal3::ToPyObject(val);
}

PyObject*
InputVector_x_count(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto val = iv_obj->mPtr->x_count();
  return PyLong_FromLong(val);
}

PyObject*
InputVector_bin_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto tmp_str = iv_obj->mPtr->bin_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_hex_str(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  auto tmp_str = iv_obj->mPtr->hex_str();
  return Py_BuildValue("s", tmp_str.c_str());
}

PyObject*
InputVector_init_method(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mPtr->init();
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
  if ( !PyVal3::FromPyObject(obj, val) ) {
    return nullptr;
  }
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mPtr->set_val(pos, val);
  Py_RETURN_NONE;
}

PyObject*
InputVector_set_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::Get(obj);
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mPtr->set_from_random(mt19937);
  Py_RETURN_NONE;
}

PyObject*
InputVector_fix_x_from_random(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!", PyMt19937::_typeobject(), &obj) ) {
    return nullptr;
  }

  auto& mt19937 = PyMt19937::Get(obj);
  auto iv_obj = reinterpret_cast<InputVectorObject*>(self);
  iv_obj->mPtr->fix_x_from_random(mt19937);
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
PyInputVector::init(
  PyObject* m
)
{
  InputVectorType.tp_name = "InputVector";
  InputVectorType.tp_basicsize = sizeof(InputVectorObject);
  InputVectorType.tp_itemsize = 0;
  InputVectorType.tp_dealloc = InputVector_dealloc;
  InputVectorType.tp_flags = Py_TPFLAGS_DEFAULT;
  InputVectorType.tp_doc = PyDoc_STR("InputVector object");
  InputVectorType.tp_methods = InputVector_methods;
  InputVectorType.tp_new = InputVector_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "InputVector", &InputVectorType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief InputVector を PyObject に変換する．
PyObject*
PyInputVector::ToPyObject(
  const InputVector& val
)
{
  auto obj = InputVectorType.tp_alloc(&InputVectorType, 0);
  auto inputvector_obj = reinterpret_cast<InputVectorObject*>(obj);
  (*inputvector_obj->mPtr) = val;
  return obj;
}

// @brief PyObject が InputVector タイプか調べる．
bool
PyInputVector::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief InputVector を表す PyObject から InputVector を取り出す．
const InputVector&
PyInputVector::Get(
  PyObject* obj
)
{
  auto inputvector_obj = reinterpret_cast<InputVectorObject*>(obj);
  return *inputvector_obj->mPtr;
}

// @brief InputVector を表すオブジェクトの型定義を返す．
PyTypeObject*
PyInputVector::_typeobject()
{
  return &InputVectorType;
}

END_NAMESPACE_DRUID
