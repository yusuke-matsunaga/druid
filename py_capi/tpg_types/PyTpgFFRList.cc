
/// @file PyTpgFFRList.cc
/// @brief PyTpgFFRList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgFFRList.h"
#include "pym/PyTpgFFRIter2.h"
#include "pym/PyTpgFFR.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgFFRList_Object
{
  PyObject_HEAD
  TpgFFRList mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgFFRList_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgFFRList::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgFFRList_Object*>(self);
  obj->mVal.~TpgFFRList();
  Py_TYPE(self)->tp_free(self);
}

Py_ssize_t
sq_length(
  PyObject* self
)
{
  auto& val = PyTpgFFRList::_get_ref(self);
  try {
    auto len_val = val.size();
    return len_val;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return -1;
  }
}

PyObject*
sq_item(
  PyObject* self,
  Py_ssize_t index
)
{
  auto& val = PyTpgFFRList::_get_ref(self);
  try {
    return PyTpgFFR::ToPyObject(val[index]);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  catch ( std::out_of_range err ) {
    std::ostringstream buf;
    buf << "out of range" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// Sequence オブジェクト構造体
PySequenceMethods sequence = {
  .sq_length = sq_length,
  .sq_item = sq_item
};

// iter 関数
PyObject*
iter_func(
  PyObject* self
)
{
  auto& val = PyTpgFFRList::_get_ref(self);
  try {
    return PyTpgFFRIter2::ToPyObject(val.iter());
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgFFRList::_get_ref(self);
  try {
    return PyBool_FromLong(val.is_valid());
  }
  catch ( std::exception err ) {
    std::ostringstream buf;
    buf << "exception" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// メソッド定義
PyMethodDef methods[] = {
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("True if valid")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief TpgFFRList オブジェクトを使用可能にする．
bool
PyTpgFFRList::init(
  PyObject* m
)
{
  TpgFFRList_Type.tp_name = "TpgFFRList";
  TpgFFRList_Type.tp_basicsize = sizeof(TpgFFRList_Object);
  TpgFFRList_Type.tp_itemsize = 0;
  TpgFFRList_Type.tp_dealloc = dealloc_func;
  TpgFFRList_Type.tp_as_sequence = &sequence;
  TpgFFRList_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgFFRList_Type.tp_doc = PyDoc_STR("Python extended object for TpgFFRList");
  TpgFFRList_Type.tp_iter = iter_func;
  TpgFFRList_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "TpgFFRList", &TpgFFRList_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgFFRList を PyObject に変換する．
PyObject*
PyTpgFFRList::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgFFRList::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgFFRList_Object*>(obj);
  new (&my_obj->mVal) TpgFFRList(val);
  return obj;
}

// @brief PyObject が TpgFFRList タイプか調べる．
bool
PyTpgFFRList::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgFFRList_Type);
}

// @brief PyObject から TpgFFRList を取り出す．
TpgFFRList&
PyTpgFFRList::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgFFRList_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgFFRList を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgFFRList::_typeobject()
{
  return &TpgFFRList_Type;
}

END_NAMESPACE_DRUID
