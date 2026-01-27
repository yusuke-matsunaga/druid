
/// @file PyTpgGateList.cc
/// @brief PyTpgGateList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyTpgGateList.h"
#include "pym/PyTpgGate.h"
#include "pym/PyUlong.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct TpgGateList_Object
{
  PyObject_HEAD
  TpgGateList mVal;
};

// Python 用のタイプ定義
PyTypeObject TpgGateList_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyTpgGateList::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<TpgGateList_Object*>(self);
  obj->mVal.~TpgGateList();
  Py_TYPE(self)->tp_free(self);
}

Py_ssize_t
sq_length(
  PyObject* self
)
{
  auto& val = PyTpgGateList::_get_ref(self);
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
  auto& val = PyTpgGateList::_get_ref(self);
  try {
    return PyTpgGate::ToPyObject(val[index]);
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

// True if valid
PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PyTpgGateList::_get_ref(self);
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


// @brief TpgGateList オブジェクトを使用可能にする．
bool
PyTpgGateList::init(
  PyObject* m
)
{
  TpgGateList_Type.tp_name = "TpgGateList";
  TpgGateList_Type.tp_basicsize = sizeof(TpgGateList_Object);
  TpgGateList_Type.tp_itemsize = 0;
  TpgGateList_Type.tp_dealloc = dealloc_func;
  TpgGateList_Type.tp_as_sequence = &sequence;
  TpgGateList_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  TpgGateList_Type.tp_doc = PyDoc_STR("Python extended object for TpgGateList");
  TpgGateList_Type.tp_methods = methods;
  if ( !PyModule::reg_type(m, "TpgGateList", &TpgGateList_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// TpgGateList を PyObject に変換する．
PyObject*
PyTpgGateList::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyTpgGateList::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<TpgGateList_Object*>(obj);
  new (&my_obj->mVal) TpgGateList(val);
  return obj;
}

// @brief PyObject が TpgGateList タイプか調べる．
bool
PyTpgGateList::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &TpgGateList_Type);
}

// @brief PyObject から TpgGateList を取り出す．
TpgGateList&
PyTpgGateList::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<TpgGateList_Object*>(obj);
  return my_obj->mVal;
}

// @brief TpgGateList を表すオブジェクトの型定義を返す．
PyTypeObject*
PyTpgGateList::_typeobject()
{
  return &TpgGateList_Type;
}

END_NAMESPACE_DRUID
