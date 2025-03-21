
/// @file PyDiffBitsArray.cc
/// @brief Python DiffBitsArray の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDiffBitsArray.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DiffBitsArrayObject
{
  PyObject_HEAD
  DiffBitsArray mVal;
};

// Python 用のタイプ定義
PyTypeObject DiffBitsArrayType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DiffBitsArray_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'DiffBitsArray' is disabled");
  return nullptr;
}

// 終了関数
void
DiffBitsArray_dealloc(
  PyObject* self
)
{
  auto dbits_obj = reinterpret_cast<DiffBitsArrayObject*>(self);
  dbits_obj->mVal.~DiffBitsArray();
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DiffBitsArray_add_output(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "k", &pos) ) {
    return nullptr;
  }

  auto& dbarray = PyDiffBitsArray::_get_ref(self);
  dbarray.add_output(pos, PV_ALL1); // 嘘！

  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DiffBitsArray_methods[] = {
  {"add_output", DiffBitsArray_add_output, METH_VARARGS,
   PyDoc_STR("add output")},
  {nullptr, nullptr, 0, nullptr}
};

Py_ssize_t
DiffBitsArray_length(
  PyObject* self
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
  return val.elem_num();
}

PyObject*
DiffBitsArray_item(
  PyObject* self,
  Py_ssize_t index
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
  try {
    int index1 = ( index >= 0 ) ? index : val.elem_num() + index;
    auto ans = val.output(index1);
    return PyBool_FromLong(ans);
  }
  catch ( std::out_of_range err ) {
    PyErr_SetString(PyExc_ValueError, err.what());
    return nullptr;
  }
}

PySequenceMethods DiffBitsArray_sequence = {
  .sq_length = DiffBitsArray_length,
  .sq_item = DiffBitsArray_item
};

// 比較関数
PyObject*
DiffBitsArray_richcompfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyDiffBitsArray::Check(self) &&
       PyDiffBitsArray::Check(other) ) {
    auto& val1 = PyDiffBitsArray::_get_ref(self);
    auto& val2 = PyDiffBitsArray::_get_ref(other);
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// ハッシュ関数
Py_hash_t
DiffBitsArray_hashfunc(
  PyObject* self
)
{
  auto& val = PyDiffBitsArray::_get_ref(self);
  return val.hash();
}

END_NONAMESPACE


// @brief 'DiffBitsArray' オブジェクトを使用可能にする．
bool
PyDiffBitsArray::init(
  PyObject* m
)
{
  DiffBitsArrayType.tp_name = "DiffBitsArray";
  DiffBitsArrayType.tp_basicsize = sizeof(DiffBitsArrayObject);
  DiffBitsArrayType.tp_itemsize = 0;
  DiffBitsArrayType.tp_dealloc = DiffBitsArray_dealloc;
  DiffBitsArrayType.tp_flags = Py_TPFLAGS_DEFAULT;
  DiffBitsArrayType.tp_doc = PyDoc_STR("DiffBitsArray object");
  DiffBitsArrayType.tp_richcompare = DiffBitsArray_richcompfunc;
  DiffBitsArrayType.tp_methods = DiffBitsArray_methods;
  DiffBitsArrayType.tp_new = DiffBitsArray_new;
  DiffBitsArrayType.tp_as_sequence = &DiffBitsArray_sequence;
  DiffBitsArrayType.tp_hash = &DiffBitsArray_hashfunc;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DiffBitsArray", &DiffBitsArrayType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DiffBitsArray を表す PyObject を作る．
PyObject*
PyDiffBitsArray::Conv::operator()(
  const DiffBitsArray& val
)
{
  auto type = PyDiffBitsArray::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto dbits_obj = reinterpret_cast<DiffBitsArrayObject*>(obj);
  new (&dbits_obj->mVal) DiffBitsArray(val);
  return obj;
}

// @brief PyObject* から DiffBitsArray を取り出す．
bool
PyDiffBitsArray::Deconv::operator()(
  PyObject* obj,
  DiffBitsArray& val
)
{
  if ( PyDiffBitsArray::Check(obj) ) {
    val = PyDiffBitsArray::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が DiffBitsArray タイプか調べる．
bool
PyDiffBitsArray::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DiffBitsArray を表す PyObject から DiffBitsArray を取り出す．
DiffBitsArray&
PyDiffBitsArray::_get_ref(
  PyObject* obj
)
{
  auto dbits_obj = reinterpret_cast<DiffBitsArrayObject*>(obj);
  return dbits_obj->mVal;
}

// @brief DiffBitsArray を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDiffBitsArray::_typeobject()
{
  return &DiffBitsArrayType;
}

END_NAMESPACE_DRUID
