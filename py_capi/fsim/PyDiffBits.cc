
/// @file PyDiffBits.cc
/// @brief Python DiffBits の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyDiffBits.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct DiffBitsObject
{
  PyObject_HEAD
  DiffBits* mPtr;
};

// Python 用のタイプ定義
PyTypeObject DiffBitsType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
DiffBits_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'DiffBits' is disabled");
  return nullptr;
}

// 終了関数
void
DiffBits_dealloc(
  PyObject* self
)
{
  auto dbits_obj = reinterpret_cast<DiffBitsObject*>(self);
  delete dbits_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
DiffBits_add_output(
  PyObject* self,
  PyObject* args
)
{
  SizeType pos = 0;
  if ( !PyArg_ParseTuple(args, "k", &pos) ) {
    return nullptr;
  }

  auto dbits_obj = reinterpret_cast<DiffBitsObject*>(self);
  auto& dbits = *dbits_obj->mPtr;
  dbits.add_output(pos);

  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef DiffBits_methods[] = {
  {"add_output", DiffBits_add_output, METH_VARARGS,
   PyDoc_STR("add output")},
  {nullptr, nullptr, 0, nullptr}
};

Py_ssize_t
DiffBits_length(
  PyObject* self
)
{
  auto& val = PyDiffBits::Get(self);
  return val.elem_num();
}

PyObject*
DiffBits_item(
  PyObject* self,
  Py_ssize_t index
)
{
  auto& val = PyDiffBits::Get(self);
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

PySequenceMethods DiffBits_sequence = {
  .sq_length = DiffBits_length,
  .sq_item = DiffBits_item
};

// 比較関数
PyObject*
DiffBits_richcompfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyDiffBits::Check(self) &&
       PyDiffBits::Check(other) ) {
    auto& val1 = PyDiffBits::Get(self);
    auto& val2 = PyDiffBits::Get(other);
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
DiffBits_hashfunc(
  PyObject* self
)
{
  auto& val = PyDiffBits::Get(self);
  return val.hash();
}

END_NONAMESPACE


// @brief 'DiffBits' オブジェクトを使用可能にする．
bool
PyDiffBits::init(
  PyObject* m
)
{
  DiffBitsType.tp_name = "DiffBits";
  DiffBitsType.tp_basicsize = sizeof(DiffBitsObject);
  DiffBitsType.tp_itemsize = 0;
  DiffBitsType.tp_dealloc = DiffBits_dealloc;
  DiffBitsType.tp_flags = Py_TPFLAGS_DEFAULT;
  DiffBitsType.tp_doc = PyDoc_STR("DiffBits object");
  DiffBitsType.tp_richcompare = DiffBits_richcompfunc;
  DiffBitsType.tp_methods = DiffBits_methods;
  DiffBitsType.tp_new = DiffBits_new;
  DiffBitsType.tp_as_sequence = &DiffBits_sequence;
  DiffBitsType.tp_hash = &DiffBits_hashfunc;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "DiffBits", &DiffBitsType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief DiffBits を表す PyObject を作る．
PyObject*
PyDiffBits::ToPyObject(
  const DiffBits& val
)
{
  auto type = _typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto dbits_obj = reinterpret_cast<DiffBitsObject*>(obj);
  dbits_obj->mPtr = new DiffBits{val};
  return obj;
}

// @brief PyObject が DiffBits タイプか調べる．
bool
PyDiffBits::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief DiffBits を表す PyObject から DiffBits を取り出す．
const DiffBits&
PyDiffBits::Get(
  PyObject* obj
)
{
  auto dbits_obj = reinterpret_cast<DiffBitsObject*>(obj);
  return *dbits_obj->mPtr;
}

// @brief DiffBits を表すオブジェクトの型定義を返す．
PyTypeObject*
PyDiffBits::_typeobject()
{
  return &DiffBitsType;
}

END_NAMESPACE_DRUID
